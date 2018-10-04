<?php
// ini_set('display_errors', 'On');
// ini_set('display_startup_errors', true);
// error_reporting(E_ALL);

include "GameController.php";
include "Board.php";
include "Draw.php";

$host = explode(":", $argv[1])[0];
$port = explode(":", $argv[1])[1];
$numOfWeights = $argv[2];
$board_length = 30;
$slow = false;

foreach ($argv as $arg) {
    if($arg == "-w"){
	$slow = true;
	echo "Slowing down game\n";
    }
}
$myController = new GameController($host, $port);
$myController->createConnection($numOfWeights, $board_length);
$myGame = new Board($board_length, $numOfWeights, 3, $myController->player1, $myController->player2);

while(!$myGame->gameOver) {
    echo "--------------------------------------------------------------------------------------------------------------\n";
    
    $sendingString = $myGame->generateSendingJSON();

    if($sendingString['move_type'] == 'place') {
        echo "Placing Stage, board state: \n";
    } else {
        echo "Removing Stage, board state: \n";
    }

    echo $sendingString['board_state'] . "\n";
    echo $sendingString['game_over'] . "\n";
    draw($myGame, false);

    $myController->send($myGame->currentTurn, $myGame->generateSendingString());
    $time1 = microtime(true);
    $move = $myController->recvMove($myGame->currentTurn);
    $time2 = microtime(true);
    $myGame->updateTime($myGame->currentTurn, $time2 - $time1);

    if($myGame->gameOver) {
        break;
    }

    if ($myGame->currentState == "place") {
        $myGame->move((int)$move->weight, (int)$move->position);
    } else {
        $myGame->remove((int)$move->position);
    }

    if ($slow) {
	   usleep(500000); // sleep for half a second
    }

}

$myController->send(1, $myGame->generateSendingString());
$myController->send(2, $myGame->generateSendingString());
draw($myGame ,true);
$myController->closeConnection();
