# disutils: language = c++
import json
from random import choice
from hps.clients import SocketClient
from time import sleep
import argparse
import game
HOST = '127.0.0.1'
PORT = 3001

class NoTippingClient(object):
    def __init__(self, name, is_first):
        self.first_resp_recv = False
        self.name = name
        self.client = SocketClient(HOST, PORT)
        self.client.send_data(json.dumps({'name': self.name, 'is_first': is_first}))
        response = json.loads(self.client.receive_data())
        self.board_length = response['board_length']
        self.num_weights = response['num_weights']
        self.myWeight = dict()
        for i in range(1, int(self.num_weights)):
            self.myWeight[i] = 1;
        self.gameState = game.PyGameState()

    def play_game(self):
        # pass
        response = {}
        while True:
            response = json.loads(self.client.receive_data())
            if 'game_over' in response and response['game_over'] == "1":
                print("Game Over!")
                exit(0)
            if not response:
                self.board_state = [0]*60
            else:
                self.board_state = board_state = [int(state) for state in filter(None,list(response['board_state'].split(' ')))]

            self.gameState.absorb(self.board_state)
            if response['move_type'] == 'place':
                position, weight = self.gameState.play()
                print("sending " + str(position) + " " + str(weight))
                self.client.send_data(json.dumps({"position": position, "weight": weight}))
            else:
                position = self.play()
                print("sending " + str(position))
                self.client.send_data(json.dumps({"position": position}))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--first', action='store_true', default=False, help='Indicates whether client should go first')
    parser.add_argument('--ip', type=str, default= 'localhost')
    parser.add_argument('--port', type=int, default= 3001)
    parser.add_argument('--name', type=str, default= "Lily")
    args = parser.parse_args()


    HOST = args.ip
    PORT = args.port

    player = NoTippingClient(args.name, args.first)
    player.play_game()

