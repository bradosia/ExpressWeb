<?php
// timer start
$debug = false;
$time_start = microtime(true);
// run executable
$executable = '/home/braduizn/public_html/example';
$headerJSONString = addslashes(json_encode($_SERVER));
exec($executable . ' "' . $headerJSONString . '" 2>&1', $outputArray, $returnVar);
// timer end
$time_end = microtime(true);
if ($returnVar == 0) {
    // success
    $body = false;
    foreach ($outputArray as $key => $value) {
            if (! $body && $value == '') {
                $body = true;
               // echo '<br>BODY<br>';
            }
            if ($body) {
                echo $value;
                //echo '<br>';
            } else {
                // header($value, true);
                 //echo $value . '<br>';
            }
    }
    
    ?>
<div style="display: block;">execution time (microseconds 10^-6): <?php echo (($time_end - $time_start) * 1000000.0) ?></div>
<?php
}
if ($returnVar != 0 || $debug) {
    // failure
    ?>
<html>
<head>
<title>PHP Test</title>
</head>
<body>
	<table>
		<tr>
			<td>Key</td>
			<td>Value</td>
		</tr>
		<tr>
			<td>exec()</td>
			<td><?php
    if (function_exists('exec')) {
        echo "enabled";
    } else {
        echo "disabled";
    }
    ?></td>
		</tr>
		<tr>
			<td>executable</td>
			<td><?php
    echo $executable;
    ?></td>
		</tr>
		<tr>
			<td>headerJSONString</td>
			<td><?php
    echo $headerJSONString;
    ?></td>
		</tr>
		<tr>
			<td>output</td>
			<td><?php
    echo var_dump($outputArray);
    ?></td>
		</tr>
		<tr>
			<td>output status</td>
			<td><?php
    /*
     * 1 Catchall for general errors let "var1 = 1/0" Miscellaneous errors, such as "divide by zero"
     * 2 Misuse of shell builtins (according to Bash documentation) Seldom seen, usually defaults to exit code 1
     * 126 Command invoked cannot execute Permission problem or command is not an executable
     * 127 "command not found" Possible problem with $PATH or a typo
     * 128 Invalid argument to exit exit 3.14159 exit takes only integer args in the range 0 - 255 (see footnote)
     * 128+n Fatal error signal "n" kill -9 $PPID of script $? returns 137 (128 + 9)
     * 130 Script terminated by Control-C Control-C is fatal error signal 2, (130 = 128 + 2, see above)
     * 255* Exit status out of range exit -1 exit takes only integer args in the range 0 - 255
     */
    echo var_dump($returnVar);
    ?></td>
		</tr>
		<tr>
			<td>execution time (microseconds 10^-6)</td>
			<td><?php
    echo (($time_end - $time_start) * 1000000.0);
    ?></td>
		</tr>
		<tr>
			<td>uname -s</td>
			<td><?php
    // exec('uname -s', $outputArray2, $returnVar2);
    echo var_dump($outputArray2);
    echo "<br>";
    echo var_dump($returnVar2);
    ?></td>
		</tr>
	</table>
</body>
</html>
<?php
}
?>