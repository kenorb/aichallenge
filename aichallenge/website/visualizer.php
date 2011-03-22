<?php include "header.php"; ?>

<div id="visualizerDiv"></div>

<?php include "footer.php"; ?>

<script type="text/javascript" src="visualizer/js/visualizer.js"></script>
<script type="text/javascript">
<?php
if (file_exists(dirname(__FILE__)."/visualizer/java")) {
	$java = "visualizer/java/";
} else {
	$java = "visualizer/applet/canvas.jar";
}
$replay = "games/" . (int)($_GET["game_id"] / 10000) . "/" . $_GET["game_id"] . ".replay";
?>
visualizer = new Visualizer(document.getElementById('visualizerDiv'), 'visualizer/', '<?php echo $java; ?>', 640, undefined);
visualizer.loadReplayDataFromURI('<?php echo $replay; ?>');
</script>
