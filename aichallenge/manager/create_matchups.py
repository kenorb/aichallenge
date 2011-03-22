#!/usr/bin/python

import logging
import logging.handlers
import math
import os
import os.path
import random
import sys
import time
from collections import defaultdict
from datetime import datetime, timedelta

import MySQLdb
from server_info import server_info

PAIRCUT_FILE = "/home/contest/pairing_cutoff"

logger = logging.getLogger('compile_logger')
logger.setLevel(logging.INFO)
_my_pid = os.getpid()
def log_message(message):
  logger.info(str(_my_pid) + ": " + message)
  print message

def get_submissions(cursor):
    cursor.execute("""SELECT MAX(leaderboard_id)
            FROM leaderboards WHERE complete=1""")
    leaderboard_id = cursor.fetchone()['MAX(leaderboard_id)']
    if leaderboard_id == None:
        leaderboard_id = 0
    log_message("latest leaderboard is %d" % leaderboard_id)
    cursor.execute("SELECT * FROM matchups")
    paired = cursor.rowcount * 2
    pending = []
    min_mid = None
    max_mid = None
    for row in cursor.fetchall():
        mid = row['matchup_id']
        if mid < min_mid or min_mid is None:
            min_mid = mid
        if mid > max_mid or max_mid is None:
            max_mid = mid
        pending.append(str(row['player_one']))
        pending.append(str(row['player_two']))
    if pending:
        pending_cond = "AND s.submission_id NOT IN (" + ",".join(pending) + ")"
        # check for matches that aren't being completed
        if (min_mid + 2000) < (max_mid - len(pending)):
            log_message("WARNING: min matchup_id is over 2000 less than max")
    else:
        pending_cond = ''
    cursor.execute("""SELECT s.*, r.rank as rank
        FROM submissions as s
        LEFT OUTER JOIN rankings as r ON r.submission_id = s.submission_id
            AND r.leaderboard_id = %s
        WHERE s.latest = 1 %s
        ORDER BY rank""" % (leaderboard_id, pending_cond))
    ranked = []
    unranked = []
    for row in cursor.fetchall():
        if row['rank'] is None:
            unranked.append(row)
        else:
            ranked.append(row)
    for group in (ranked, unranked):
        for sub in group:
            s_id = sub['submission_id']
            cursor.execute("""SELECT count(1) FROM games
                WHERE player_one = %s OR player_two = %s""" % (s_id, s_id))
            sub['game_count'] = cursor.fetchone()['count(1)']
    log_message("Got %d ranked and %d unranked active submissions"
            % (len(ranked), len(unranked)))
    return (ranked, unranked, paired)

def get_total_ranking(cursor, ranked, unranked):
    ranking = defaultdict(list)
    ranking[0] = [] # assure one key for middle rank calculation
    for sub in ranked:
        ranking[sub['rank']].append(sub)
    no_previous = []
    for sub in unranked:
        log_message("Finding provisional rank for submission %d"
                % sub['submission_id'])
        no_previous.append(sub)
        log_message("  could not find a previous rank")
    if no_previous:
        middle_rank = max(ranking.keys()) / 2
        log_message("Setting %d submissions with no provisional rank to %d"
                % (len(no_previous), middle_rank))
        ranking[middle_rank] += no_previous
    total_ranking = []
    ranking = ranking.items()
    ranking.sort()
    for rank, tier in ranking:
        random.shuffle(tier)
        total_ranking += tier
    return total_ranking

def get_player_one_order(total_ranking):
    play_counts = defaultdict(list)
    for sub in total_ranking:
        play_counts[sub['game_count']].append(sub)
    play_counts = play_counts.items()
    play_counts.sort(reverse=True)
    order = []
    for count, subs in play_counts:
        random.shuffle(subs)
        order += subs
    return order

def get_previous_opponents(cursor, p, num_opponents):
    cursor.execute("""(SELECT player_two as opponent, timestamp FROM games
                WHERE player_one = %(id)s)
            UNION
            (SELECT player_one as opponent, timestamp FROM games
                WHERE player_two = %(id)s)
            ORDER BY timestamp desc
            LIMIT %(num)d""" % {'id': p['submission_id'], 'num': num_opponents})
    return [r['opponent'] for r in cursor]

def choose_opponent(p1, ranking, recent_ids):
    orig_ranking = ranking
    if p1['last_game_timestamp'] == None:
        ranking = [s for s in ranking
                if s == p1 or s['last_game_timestamp'] != None]
    recent_ids = set(recent_ids)
    ranking = [s for s in ranking if s['submission_id'] not in recent_ids]
    if len(ranking) < 2:
        ranking = orig_ranking
        #raise ValueError("No opponents left after removing recent, %s\n%s" % (
        #    p1['submission_id'], ranking))
    p1_ix = ranking.index(p1)
    p2_ix = p1_ix
    while p1_ix == p2_ix:
        offset = random.paretovariate(0.75)
        offset = int(offset if random.randint(0,1) else 0-offset)
        ix = p1_ix + offset
        if ix < 0 or ix > len(ranking) - 1:
            continue
        p2_ix = ix
    return ranking[p2_ix]

_SERVER_MAPS = None
def choose_map(cursor, player1, player2):
    global _SERVER_MAPS
    if not _SERVER_MAPS:
        cursor.execute("SELECT map_id, priority FROM maps")
        _SERVER_MAPS = {}
        for row in cursor.fetchall():
            if row['priority'] > 0:
                _SERVER_MAPS[row['map_id']] = row['priority']
    p1_id = player1['submission_id']
    p2_id = player2['submission_id']
    cursor.execute("""SELECT map_id FROM games
        WHERE (player_one = %s AND player_two = %s)
            OR (player_one = %s AND player_two = %s)""" %
            (p1_id, p2_id, p2_id, p1_id))
    counts = dict()
    for map_id in _SERVER_MAPS.keys():
        counts[map_id] = 0
    for row in cursor.fetchall():
        try:
            counts[row['map_id']] += 1
        except KeyError:
            # there are a large number of games recorded in the contest
            # database with a bad map id
            pass
    min_maps, min_played = counts.popitem()
    min_maps = [min_maps]
    for map_id, plays in counts.items():
        if plays < min_played:
            min_played = plays
            min_maps = [map_id]
        elif plays == min_played:
            min_maps.append(map_id)
    if min_played > 0:
        log_message("WARNING: All maps have been played with this pairing")
    qualified_maps = [min_maps.pop()]
    high_priority = _SERVER_MAPS[qualified_maps[0]]
    for map_id in min_maps:
        if _SERVER_MAPS[map_id] < high_priority:
            high_priority = _SERVER_MAPS[map_id]
            qualified_maps = [map_id]
        elif _SERVER_MAPS[map_id] == high_priority:
            qualified_maps.append(map_id)
    qmaps_vstr = ",".join(str(m) for m in qualified_maps)
    cursor.execute("""SELECT map_id, count(1) FROM games
        WHERE map_id in (%s) AND (player_one = %d or player_two = %d)
        GROUP BY map_id"""
        % (qmaps_vstr, p1_id, p1_id))
    p1_counts = defaultdict(int)
    for row in cursor.fetchall():
        p1_counts[row['map_id']] = row['count(1)']
    cursor.execute("""SELECT map_id, count(1) FROM games
        WHERE map_id in (%s) AND (player_one = %d or player_two = %d)
        GROUP BY map_id"""
        % (qmaps_vstr, p2_id, p2_id))
    p2_counts = defaultdict(int)
    for row in cursor.fetchall():
        p2_counts[row['map_id']] = row['count(1)']
    comb_counts = {}
    for map_id in qualified_maps:
        comb_counts[map_id] = p1_counts[map_id] + p2_counts[map_id]
    least_played = [qualified_maps.pop()]
    low_count = comb_counts[least_played[0]]
    for map_id in qualified_maps:
        if comb_counts[map_id] < low_count:
            low_count = comb_counts[map_id]
            least_played = [map_id]
        elif comb_counts[map_id] == low_count:
            least_played.append(map_id)
    match_map = random.choice(least_played)
    info_str = "%s(%d) p1 %s p2 %s" % (match_map,
            _SERVER_MAPS[match_map], p1_counts[match_map], p2_counts[match_map])
    return (match_map, info_str)

def add_matches(cursor, max_matches, pairing_cutoff):
    ranked, unranked, paired = get_submissions(cursor)
    total_ranking = get_total_ranking(cursor, ranked, unranked)
    if pairing_cutoff:
        pairing_cutoff = max(pairing_cutoff - paired, 0)
        total_ranking = total_ranking[:pairing_cutoff]
    log_message("After cutoff %d available for pairing"
            % (len(total_ranking),))
    player_order = get_player_one_order(total_ranking)
    num_matches = 0
    matchup_values = []
    pairing_start = time.time()
    while len(total_ranking) > 1 and num_matches < max_matches:
        p1 = player_order.pop()
        recent = get_previous_opponents(cursor, p1, min(50, len(total_ranking) - 2))
        p2 = choose_opponent(p1, total_ranking, recent)
        m, map_info = choose_map(cursor, p1, p2)
        p1_rank = p1['rank'] if p1['rank'] else -1
        p2_rank = p2['rank'] if p2['rank'] else -1
        log_message("%s (%d, %d) plays %s (%d, %d) on %s" % (
                p1['submission_id'], p1_rank, p1['game_count'],
                p2['submission_id'], p2_rank, p2['game_count'],
                map_info))
        matchup_values.append("(%d,%d,%d)" % (
            p1['submission_id'], p2['submission_id'], m))
        total_ranking.remove(p1)
        total_ranking.remove(p2)
        player_order.remove(p2)
        num_matches += 1
    if matchup_values:
        cursor.execute("""INSERT matchups (player_one, player_two, map_id)
            VALUES %s""" % (",".join(matchup_values),))
    pairing_time = time.time() - pairing_start
    return (num_matches, pairing_time)

def get_queue_size(cursor):
    cursor.execute("""SELECT count(*) FROM matchups
        WHERE dispatch_time IS NULL""")
    return cursor.fetchone()['count(*)']

def main(run_time=0, high_buffer=8, low_buffer=4):
    start_time = time.time()
    try:
        handler = logging.handlers.RotatingFileHandler("matchup.log",
                                               maxBytes=1000000,
                                               backupCount=5)
        logger.addHandler(handler)
    except IOError:
       # couldn't start the file logger
       pass
    while True:
        if os.path.exists(PAIRCUT_FILE):
            pc_file = open(PAIRCUT_FILE, 'r')
            pairing_cutoff = int(pc_file.readline())
            pc_file.close()
        else:
            pairing_cutoff = None
        try:
            connection = MySQLdb.connect(host = server_info["db_host"],
                                     user = server_info["db_username"],
                                     passwd = server_info["db_password"],
                                     db = server_info["db_name"])
        except MySQLdb.OperationalError:
            log_message("WARNING: Received mysql OperationalError")
            time.sleep(10)
            continue
        cursor = connection.cursor(MySQLdb.cursors.DictCursor)
        try:
            cursor.execute("""SELECT count(*)/2 as gpm FROM games
                    WHERE timestamp > (NOW() - INTERVAL 2 minute)""")
            gpm = max(float(cursor.fetchone()['gpm']), 5.)
            queue_size = get_queue_size(cursor)
            log_message("Found %d matches in queue with %d gpm"
                    % (queue_size, gpm))
            if (queue_size < gpm * low_buffer
                    or time.time() - start_time >= run_time):
                start_adding = time.time()
                num_added = 0
                pair_time = 0
                num_matches = max((gpm * high_buffer) - queue_size, 20)
                while num_matches >= 20:
                    batch_size = min(num_matches + (gpm * 0.5), 100)
                    batch_added, batch_time = add_matches(cursor, batch_size,
                            pairing_cutoff)
                    num_added += batch_added
                    pair_time += batch_time
                    if batch_added < batch_size:
                        break # we paired everyone that could be
                    queue_size = get_queue_size(cursor)
                    num_matches = (gpm * high_buffer) - queue_size
                log_message("Added %d new matches to queue in %.2f seconds (%.2f in pairing)"
                        % (num_added, time.time()-start_adding, pair_time))
                if time.time() - start_time >= run_time:
                    break
        finally:
            cursor.close()
            connection.close()
        buffertime = max(1, ((queue_size / float(gpm)) - 2.1) * 60)
        time.sleep(min(30, buffertime))

if __name__ == '__main__':
    runtime = 0
    low_buffer = 3
    high_buffer = 7
    if len(sys.argv) > 1:
        runtime = int(sys.argv[1])
    if len(sys.argv) > 2:
        high_buffer = float(sys.argv[2])
    if len(sys.argv) > 3:
        low_buffer = float(sys.argv[3])
    main(runtime, high_buffer, low_buffer)

