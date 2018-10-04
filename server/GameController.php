<?php

class GameController {
    private $socket;
    private $resources;
    public $player1 = "Player 1";
    public $player2 = "Player 2";

    function __construct($address, $port) {
        $this->socket = socket_create(AF_INET, SOCK_STREAM, 0);
        socket_bind($this->socket, $address, $port);
    }

    function createConnection($numOfWeights, $board_length) {
        socket_listen($this->socket);
        $intial_message = json_encode(['num_weights' => $numOfWeights, 'board_length' => $board_length]);
         
         echo "Waiting for Player...\n";
         while (true) {
             if ($this->resources[1] = socket_accept($this->socket)) {
                echo "Socket Accepted...\n";
                 socket_set_nonblock($this->resources[1]);
                 echo "Waiting For message to come...\n";
                 $data = json_decode($this->recv(1));
                 $this->player1 = $data->name;
                 $this->send(1, $intial_message);
                 echo "Connection from " . $this->player1 . ", established\n";
                 break;
             }
         }

        echo "Waiting for Player...\n";
        while (true) {
            if ($this->resources[2] = socket_accept($this->socket)) {
                socket_set_nonblock($this->resources[2]);
                $data = json_decode($this->recv(2));
                $this->player2 = $data->name;
                $this->send(2, $intial_message);
                echo "Connection from " . $this->player2 . ", established\n";
                break;
            }
        }

        if($data->is_first == 1) {
            $tmp = $this->resources[1];
            $this->resources[1] = $this->resources[2];
            $this->resources[2] = $tmp;

            $tmp = $this->player1;
            $this->player1 = $this->player2;
            $this->player2 = $tmp;
        }
    }

    function closeConnection() {
        socket_close($this->resources[1]);
        socket_close($this->resources[2]);
        socket_close($this->socket);
    }

    function send($player, $string) {
        socket_write($this->resources[$player], "$string\n");
    }

    function recvMove($player) {
        $data = json_decode($this->recv($player));
        $player = $player == '1' ? $this->player1 : $this->player2;

        echo "Received move from " . $player . "\n";
        return $data;
    }

    function recv($player) {
        while (true) {
            $data = socket_read($this->resources[$player], 1024, PHP_BINARY_READ);
            if ($data != "") {
                return $data;
            }
        }
    }
}
