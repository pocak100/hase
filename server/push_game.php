<?php 
include 'config.php';
include 'utils.php';

$connection = mysql_connect ($mysql_host, $mysql_username, $mysql_password) or die;
mysql_select_db($mysql_dbname) or die;

$game_id = (int)$_POST['game_id'];
$player_id = (int)$_POST['player_id'];
$player_pw = (int)$_POST['player_pw'];
$second_of_player = (int)$_POST['second_of_player'];
if (array_key_exists('gzip',$_POST))
	$data = escape_input(gzdecode($_POST['data']));
else
	$data = escape_input($_POST['data']);


$query = "SELECT * FROM " . $mysql_prefix . "player_list WHERE game_id = '$game_id' AND  player_id = '$player_id'";
$result = mysql_query($query) or die;
$row = mysql_fetch_assoc( $result );

if ($row['status'] == -2) //killed...
	echo "error: 1";
else
{
	$now = time();
	if ($row['heartbeat_time'] < $now-90) //killed...
	{
		$query = "UPDATE " . $mysql_prefix . "player_list SET status='-2' WHERE game_id = '$game_id' AND player_id = '$player_id'";
		mysql_query($query) or die;		
		echo "error: 1";
	}
	else
	{
		if ($row['player_pw'] == $player_pw)
		{
			//saving push
			$query = "INSERT INTO " . $mysql_prefix . "data_list (game_id, player_id, second_of_player, data) ".
			"VALUES ( '$game_id', '$player_id', '$second_of_player', '$data' )";
			if (!mysql_query($query))
			{
				$query = "UPDATE " . $mysql_prefix . "data_list SET data='$data' WHERE game_id = '$game_id' AND player_id = '$player_id' AND second_of_player = '$second_of_player'";
				mysql_query($query) or die;
			}

			//player heartbeat
			$query = "UPDATE " . $mysql_prefix . "player_list SET heartbeat_time='$now' WHERE game_id = '$game_id' AND player_id = '$player_id'";
			mysql_query($query) or die;

			//game heartbeat
			$query = "UPDATE " . $mysql_prefix . "game_list SET create_date='$now' WHERE game_id = '$game_id'";
			mysql_query($query) or die;
		}	
		echo "error: 0";
	}
}
mysql_close($connection);
?>
