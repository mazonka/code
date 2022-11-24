<html>

<head>
	<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
	<title>Hasq virual interview</title>
	<meta http-equiv="Content-Language" content="en-us" />
	
	<link rel="stylesheet" href="jquery/css/smoothness/jquery-ui-1.8.20.custom.css">
	<link rel="stylesheet" href="style.css">
</head>

<script src="jquery/js/jquery-1.7.2.min.js"></script>
<script src="jquery/js/jquery-ui-1.8.20.custom.min.js"></script>

<body>

<div style="height:1px;background:#000;font-size:1px;">
...
</div>

<div id="page-container" align=left>



<style>.butt { background-color: rgb(248,216,185); border: 1px red solid; }</style>

<br/><br/><br/><br/>

<?php


require "wzq.php";

error_reporting(0);
$file="wz/".$_REQUEST["u"];

if( $_REQUEST["make"] == "yes" )
{
	mkdir("wz");
	file_put_contents($file,"0\n");
        die("User created");
}

$msg=$_REQUEST["msg"];
$step=file_get_contents($file) or die("No such user");
error_reporting(-1);
$step=trim($step);


date_default_timezone_set('UTC');
if( $step > 0 ){
	$data="\n\n***** Q".($step-1)." **** "
		.date('l jS \of F Y h:i:s A')
		."\n".$qs[$step-1]."\n**********\n".$msg."\n";
	file_put_contents($file.".dat",$data,FILE_APPEND);
}

$stepmax=count($qs)-1;
if( $step > $stepmax ) die("Thank you for your time");

echo "Question ".$step." of ".$stepmax."<br/><br/>";
echo "<p style='color:black;font-size:18px;'>".$qs[$step]."</p>";

$step=$step+1;
file_put_contents($file,$step);

?>

<form name='wz' id='wz' action="wz.php" method="post">

<br/><br/>

<textarea name="msg" id="msg" rows=6 cols=50 style="border:1px solid black;"></textarea>
<br/><br/>
<input class='butt' type='submit' value='NEXT'/>
<?php
echo "<input type='hidden' name='u' value='". $_REQUEST["u"] ."' />";
?>

</form>


  <div id="prefooter"></div>
  <div id="footer">
  </div>

</div>

</body>

</html>
