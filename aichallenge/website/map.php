<?php

require_once("header.php");
require_once("visualizer_widget.php");
require_once("game_list.php");
require_once("lookup.php");

$map = str_replace("%2F", "/", filter_input(INPUT_GET, 'map', FILTER_SANITIZE_ENCODED));
$map_row = get_map_row($map);

if ($map_row !== NULL) {
    echo "<a href=\"map/".$map_row['filename']."\">".$map_row['filename']."</a> (download link)";
    visualize_map($map_row['filename']);    
    echo get_game_list_table(1, NULL, NULL, $map_row['map_id'], FALSE, 'map.php');
} else {
    $map = htmlentities($map);
    echo "<p>Map $map does not exist</p>";
}


require_once("footer.php");

?>
