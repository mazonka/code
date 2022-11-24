<html>

<head>
	<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
	<title>23 Lab Departure time</title>
	<meta http-equiv="Content-Language" content="en-us" />
	
	<link rel="stylesheet" href="style.css">
</head>

<?php

$ip = $_SERVER['REMOTE_ADDR'];
if( !empty($_REQUEST) && $_REQUEST["ip"] != "" )
    $ip = $_REQUEST["ip"];

$options = array("?","before 1700","1700","1715","1730","1745"
    ,"1800","1815","1830","1845","1900");

function prnargs()
{
    if(!empty($_REQUEST)){
        foreach($_REQUEST as $x => $y){
            $z = str_replace('<', '&lt;', str_replace('>', '&gt;', $y));
            $z = str_replace("\n", '<br>', $z);
                echo $x." = ".$z."<br/>\n";
        }
    }
    echo "<hr/>\n";
}

//prnargs();

date_default_timezone_set ("Australia/Adelaide");
$today=getdate();
$today6=(string)(($today[year]-2000)*100+$today[mon])*100+$today[mday];
$deptime=0;

if( !empty($_REQUEST) && $_REQUEST["name"] != "" )
{
    resetname($ip,$_REQUEST["name"]);
}

$nms = loadnames();
$nm=$ip;
if( array_key_exists($ip,$nms) )
    $nm = $nms[$ip];
else
    $nms[$ip] = $nm;

if( !empty($_REQUEST) && $_REQUEST["time"] != "" )
{
    resettime($ip,$_REQUEST["time"]);
}

$tms = loadtimes();
$tm=0;
if( array_key_exists($ip,$tms) ) $tm = (int)$tms[$ip];

$maxtime=-1;
calcDeptime();

?>

<form name='form' id='form' action="index.php" method="post">

<table border=1>
<tr><td>The final departure time for today is:</td>
<td bgcolor='#C0F0A0'><font size='6'><?php echo $options[$deptime]; ?></font></td></tr>
</table>

<br/>

<table border=1>
<?php
	foreach ($nms as $k => $v)
	{
        //if( $v != "" )
        echo "<tr>";
        if( $k == $ip )
        {
            echo "<td><input type='text' name='name' value='".$nm."'/></td>";
            echo "<td>".selector($tm)."</td>";
        }
        else
        {
            echo "<td>".$v."</td>";
            if( array_key_exists($k,$tms) )
            {
                if( $tms[$k] == $deptime )
                    echo "<td bgcolor='#C0F0A0'>";
                else if( $tms[$k] == $maxtime )
                    echo "<td bgcolor='#F0A0C0'>";
                else
                    echo "<td>";
                echo $options[$tms[$k]]."</td>";
            }
            else
                echo "<td></td>";
        }
        echo "</tr>";
	}
?>
</table>

<br/>

<!--
<table border=1>
<tr><td>Your name</td>
<?php
echo "<td><input type='text' name='name' value='".$nm."'/></td>";
?>
<td><input type="submit" name="ok" value="Set" /></td></tr>
</table>
-->


</form>

<?php
//echo $ip;
function dbug()
{
global $ip, $today6, $_REQUEST, $maxtime;
echo "ip=[".$ip."] ";
echo $_REQUEST["name"];
echo $today6;
echo $maxtime;
}
//dbug();

function resetname($ip,$name)
{
    $names = loadnames();

	if( strlen($name) > 8 && substr($name,0,6) == "delete" )
    {
        $nm = substr($name,7);
    	foreach ($names as $k => $v)
    	{
            if( $v == $nm ) $names[$k] = "";
        }
        $name = "delete";
    }

    $names[$ip] = $name;
    if( $name == "delete" )
        $names[$ip] = "";

    savenames($names);
}

function resettime($ip,$time)
{
    $times = loadtimes();

    $times[$ip] = $time;

    savetimes($times);
}

function savefl($names,$fl)
{
	$fp = fopen($fl.".tmp","w");

	foreach ($names as $k => $v)
	{
        if( $v != "" )
    		fwrite($fp, $k." ".$v."\n");
	}

	fclose($fp);

    rename($fl.".tmp",$fl);
}

function savenames($names)
{
    savefl($names,"names.txt");
}

function loadnames()
{
    return loadfl("names.txt");
}

function savetimes($times)
{
    global $today6;
    savefl($times,"times.".$today6.".txt");
}

function loadtimes()
{
    global $today6;
    return loadfl("times.".$today6.".txt");
}

function loadfl($fl)
{
	$db = array();

    if( file_exists($fl) )
    {
	   $fp = fopen($fl,"r");
	   for( $i=0; $i<1000; $i=$i+1 )
    	{
        	$em = stream_get_line($fp,1024," ");
		$em = trim($em);
        	if( $em=="" ) break;
        	$ln = stream_get_line($fp,1024,"\n");
		$ln = trim($ln);
        	if( $ln=="" ) break;
		while( substr($ln,0,1) == " " ) $ln = substr($ln,1);
		if( $ln=="" ) $ln = "*";
        	$db[$em] = $ln;
	   }
    }

	return $db;
}

function selector($tm)
{
 global $options;
 $r = " <select name='time' onchange='selected()'>";
	foreach ($options as $k => $v)
	{
        if( $k == $tm )
        $r = $r."<option value='".$k."' selected>".$v."</option>";
        else
        $r = $r."<option value='".$k."'>".$v."</option>";
    }
 $r =$r."</select>";
 return $r;
}

function calcDeptime()
{
    global $deptime, $tms, $options, $maxtime;

    $ar = array();

	foreach ($tms as $k => $v)
	{
        array_push($ar,$v);
    }

    $sz = sizeof($ar);
    if( $sz < 2 ) return;

    sort($ar);

    $deptime = $ar[$sz-2];
    $maxtime = $ar[$sz-1];
}


?>

<script>
function selected()
{
    document.getElementById("form").submit();
}
</script>

<a href="/">Reload</a> <br/>

<small>To add name: type your name in the text field (instead of IP address)
<br/>
To remove name: type "delete" or "delete <i>name</i>", e.g. delete Julia Piotto
<br/>

<?php
echo "".date('d/m/Y h:i:s a', time());
?>

</small>

</html>
