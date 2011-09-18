<?php
// ini_set('error_reporting', E_ALL);
// ini_set('display_errors', true);

require_once('api_functions.php');
require_once('server_info.php');
$skills_dir = $server_info["repo_path"] . "/manager/PHPSkills/Skills/";
require_once($skills_dir.'TrueSkill/FactorGraphTrueSkillCalculator.php');
require_once($skills_dir.'GameInfo.php');
require_once($skills_dir.'Player.php');
require_once($skills_dir.'Rating.php');
require_once($skills_dir.'Team.php');
require_once($skills_dir.'Teams.php');
require_once($skills_dir.'SkillCalculator.php');

use Moserware\Skills\TrueSkill\FactorGraphTrueSkillCalculator;
use Moserware\Skills\GameInfo;
use Moserware\Skills\Player;
use Moserware\Skills\Rating;
use Moserware\Skills\Team;
use Moserware\Skills\Teams;
use Moserware\Skills\SkillCalculator;


header("Content-type: application/json");

$json_string = file_get_contents('php://input');
$json_hash = md5($json_string);
$gamedata = json_decode($json_string);

function game_result_error ($message) {
    // this function is used to store the game result json somewhere
    // in case of failure
    global $server_info;
    global $json_string;
	$message = sprintf("%s - %s", date(DATE_ATOM), $message) . "\n";
    $message .= $json_string . "\n\n";
    error_log($message, 3, $server_info["game_result_errors"]);
    die();
}

if ($gamedata == null) {
    game_result_error("Did not recieve post data for game result as proper json.");
}   

// always return received hash so worker can move on to next task
echo json_encode(array( "hash" => $json_hash ));

// confirm matchup_id
$confirm_result = contest_query("select_matchup_confirm",
                                $gamedata->matchup_id);
$confirm_worker_id = NULL;
if ($confirm_result) {
    while ($confirm_row = mysql_fetch_assoc($confirm_result)) {
        $confirm_worker_id = $confirm_row["worker_id"];
    }
} else {
    game_result_error("Error confirming matchup ".strval($gamedata->matchup_id)." is owned by ".$worker['worker_id']);
}
if ($confirm_worker_id == NULL or $confirm_worker_id != $worker['worker_id']) {
    api_log(sprintf("Game result posted does not belong to worker: %s belongs to %s, not %s",
                    $gamedata->matchup_id,
                    $confirm_worker_id,
                    $worker['worker_id']));
    die();
}

if (array_key_exists('error', $gamedata)) {
    // set to non-existant worker and email admin
    if (contest_query('update_matchup_failed',
                      json_encode($gamedata->error),
                      $gamedata->matchup_id)) {
        // echo json_encode(array( "hash" => $json_hash ));
        api_log('worker '.$worker['worker_id'].' ('.$worker['ip_address'].') posted failed matchup '.$gamedata->matchup_id.":\n".$gamedata->error."\n".json_encode($gamedata));
    } else {
        game_result_error("Error updating failed matchup ".$gamedata->matchup_id);
    }
} else {
    // move matchup data to game table
    if (!contest_query("insert_game_data",
                       $gamedata->replaydata->turns,
                       $gamedata->game_length,
                       $gamedata->replaydata->cutoff ? '1' : '0',
                       $gamedata->replaydata->winning_turn,
                       $gamedata->replaydata->ranking_turn,
                       $gamedata->matchup_id)) {
        api_log(sprintf("Error updating game table for matchup %s",
                        $gamedata->matchup_id));
        api_log(mysql_error());
        // mysql_query("ROLLBACK;");
        die();
    }
    $game_id = mysql_insert_id();
    // calculate new trueskill values
    $skill_result = contest_query("select_matchup_players", $gamedata->matchup_id);
    if (!$skill_result) {
        game_result_error("Error retrieving matchup ".strval($gamedata->matchup_id)." player data for trueskill calculation");
    }
    try {
        $calculator = new FactorGraphTrueSkillCalculator();
        $game_info = new GameInfo(50.0,       // mu
                                  50.0/3.0,   // sigma
                                  50.0/6.0,   // beta
                                  50.0/300.0, // tau
                                  0.05);      // draw prob
        $ratings = array();
        $players = array();
        $teams = array();
        while ($player_row = mysql_fetch_assoc($skill_result)) {
            $player_id = $player_row['player_id'];
            $ratings[] = new Rating($player_row['mu'], $player_row['sigma']);
            $players[] = new Player($player_id + 1);
            $teams[] = new Team($players[$player_id], $ratings[$player_id]);
        }
        $new_ratings = $calculator->calculateNewRatings($game_info, $teams, $gamedata->rank);
    } catch (Exception $e) {
        game_result_error(json_encode($e));
    }
    for ($player_id = 0, $size = sizeof($gamedata->rank); $player_id < $size; ++$player_id) {
        $new_rating = $new_ratings->getRating($players[$player_id]);
        if (!contest_query("insert_game_player",
                           $game_id,
                           $gamedata->errors[$player_id],
                           $gamedata->status[$player_id],
                           $gamedata->rank[$player_id],
                           $gamedata->score[$player_id],
                           $ratings[$player_id]->getMean(),
                           $new_rating->getMean(),
                           $ratings[$player_id]->getStandardDeviation(),
                           $new_rating->getStandardDeviation(),
                           $gamedata->matchup_id,
                           $player_id)) {
            game_result_error(sprintf("Error updating game players for matchup %s",
                            $gamedata->matchup_id)."\n".mysql_error());
        }
    }

    if (!contest_query("delete_matchup_player", $gamedata->matchup_id)) {
        api_log(sprintf("Error deleting players for matchup %s",
                        $gamedata->matchup_id)."\n".mysql_error());
    }
    if (!contest_query("delete_matchup", $gamedata->matchup_id)) {
        api_log(sprintf("Error deleting matchup %s",
                        $gamedata->matchup_id)."\n".mysql_error());
    }
    if (!contest_query("update_submission_trueskill", $game_id)) {
        api_log(sprintf("Error updating submision trueskill for game %s",
                        $game_id)."\n".mysql_error());
    }
    $mysqli = new MySQLI($db_host, $db_username, $db_password, $db_name);
    $mysqli->multi_query("call update_rankings($game_id);");
    while ($mysqli->more_results() && $mysqli->next_result());
    $mysqli->close();

    // update game data with meta data
    $gamedata->playernames = array();
    $gamedata->submission_ids = array();
    $gamedata->user_ids = array();
    $gamedata->challenge_rank = array();
    $gamedata->challenge_skill = array();
    $result = contest_query("select_game_metadata",
                            $game_id);
    if ($result) {
        while ($meta_row = mysql_fetch_assoc($result)) {
            $gamedata->playernames[] = $meta_row["username"];
            $gamedata->submission_ids[] = $meta_row["submission_id"];
            $gamedata->user_ids[] = $meta_row["user_id"];
            $gamedata->challenge_rank[] = $meta_row["rank"];
            $gamedata->challenge_skill[] = $meta_row["skill"];
        }
    }
    $gamedata->user_url = "http://" . $gamedata->location . "/profile.php?user=~";
    $gamedata->game_url = "http://" . $gamedata->location . "/visualizer.php?game=~";
    $gamedata->date = date(DATE_ATOM);
    $gamedata->game_id = $game_id;
    $gamedata->worker_id = $worker['worker_id'];
    // errors may contain sensitive info, such as code tracebacks
    unset($gamedata->errors);
    // create pathname to replay file
    $replay_dir = $server_info["replay_path"] . "/" . strval((int) ($game_id / 1000000)) . "/" . strval((int) (($game_id / 1000) % 1000));
    if (!file_exists($replay_dir)) {
        // api_log("Replay dir: " . $replay_dir);
        mkdir($replay_dir, 0775, true);
    }
    $replay_filename = $replay_dir . "/" . $game_id . ".replaygz";
    // api_log("Replay file: " . $replay_filename);
    try {
        // api_log("Cwd: " . getcwd());
        $replay_file = fopen($replay_filename, 'w') or api_log(json_encode(error_get_last()));
        fwrite($replay_file, gzencode(json_encode($gamedata), 9));
        fclose($replay_file);
        chmod($replay_filename, 0664);
        // put game id in memcache for front page
        if ($memcache) {
            $memcache->set('last_game_id', $game_id);
        }
        api_log('worker '.$worker['worker_id'].' ('.$worker['ip_address'].') posted matchup '.$gamedata->matchup_id.' game '.$game_id);
    } catch (Exception $e) {
        api_log(json_encode($e));
    }
}

?>
