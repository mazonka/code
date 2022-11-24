
<?php
$dir    = './';
$files = scandir($dir);

echo "<ul>";

foreach($files as $file)
{
	if( $file == "." ){}
	else if( $file == "index.php" ){}
	else if( $file == "dir.php" ){}
	else
	echo "<li><a href=$file>".basename($file)."</a></li>";
}
echo "</ul>";

?>
