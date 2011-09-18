<?php
include('header.php');
require_once('ranking.php');
//include_once 'rankings_widget.php';

echo <<<EOT
<h2>Current Rankings</h2>
<p>These rankings are updated continuously, 24 hours a day. Want to see your
  name on the leaderboard? Check out the <a href="quickstart.php">
  Five Minute Quickstart Guide</a>!</p>
EOT;

//echo getRankingsTableString(1, false, 100, "");

$page = get_type_or_else("page", FILTER_VALIDATE_INT, 1);

echo get_ranking_table($page);

echo '
<script>
$(function () {
    $(".ranking").tablesorter({
        textExtraction: function (node) {
            node = $(node);
            if (node.attr("class") === "number") {
                return parseFloat(node.text());
            } else if (node.attr("class") === "country") {
                return node.children().children().attr("title");
            } else {
                return node.text();
            }
        }
    });
});
</script>';
include 'footer.php';
?>
