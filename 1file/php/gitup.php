<html>
<body>
<pre>
<?php

date_default_timezone_set('UTC');
$datae = $_GET['data'];
$datad = date('ljSFYA');
if( $datae != $datad )
{
	echo "data=".$datad."\n";
//	exit();
}


$cwd = getcwd();
echo $cwd . "\n";

$c = "/home/content/21/12333821";
$k = exec("ls $c",$r);
print_r($r);
echo "===========\n";
$lib = "GIT_EXEC_PATH=$c/git-core";
echo $lib."\n";
$k = putenv($lib);
echo $k."\n";
unset($r);


echo "===========\n";
//$k = exec("which git 2> z",$r);
$k = exec("/home/content/21/12333821/bin/git pull 2> z",$r);
print_r($r);
echo "== Error {\n";
echo file_get_contents("z");
echo "}\n";

?>
</pre>
</body>
</html>

