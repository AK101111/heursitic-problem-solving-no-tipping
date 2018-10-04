import json
from random import choice, randint
from hps.clients import SocketClient
from time import sleep
import argparse
HOST = 'localhost'
PORT = 3000


class NoTippingClient(object):
    def __init__(self, name, is_first):
        self.first_resp_recv = False
        self.name = name
        self.client = SocketClient(HOST, PORT)
        self.client.send_data(json.dumps({'name': self.name, 'is_first': is_first}))
        response = json.loads(self.client.receive_data())
        self.board_length = int(response['board_length'])
        self.num_weights = int(response['num_weights'])
        self.myWeight = dict()
        for i in range(1, int(self.num_weights) + 1):
            self.myWeight[i] = 1;

    def play_game(self):
        # pass
        response = {}
        while True:
            response = json.loads(self.client.receive_data())
            if 'game_over' in response and response['game_over'] == "1":
                print("Game Over!")
                exit(0)

            # Indices start at zero but are actually
            board_state = [int(state) for state in filter(None, list(response['board_state'].split(' ')))]

            if response['move_type'] == 'place':
                position, weight = self.place(board_state)
                self.client.send_data(json.dumps({"position": position, "weight": weight}))
            else:
                position = self.remove(board_state)
                self.client.send_data(json.dumps({"position": position}))
                
            
            

    def place(self, board_state):
        """
        PLACE YOUR PLACING ALGORITHM HERE
        
        Inputs:
        board_state - array of what weight is at a given position on the board

        Output:
        position (Integer), weight (Integer)
        """
        position = randint(-self.board_length, self.board_length)
        weight = self.myWeight[randint(1, self.num_weights)]
        num_tries = 0
        self.check_balance(self.copy_board_with_updated_move(board_state, position, weight))
        # import pdb; pdb.set_trace()
        while board_state[(position + self.board_length) % self.board_length] != 0 and self.myWeight[weight] != 1 and num_tries < 10 and not self.check_balance(self.copy_board_with_updated_move(board_state, position, weight)):
            import pdb; pdb.set_trace()
            position = randint(-self.board_length, self.board_length)
            weight = self.myWeight[randint(1, self.num_weights)]
            num_tries += 1

        self.myWeight[weight] = 0

        return position, weight

    def remove(self, board_state):
        """
        PLACE YOUR REMOVING ALGORITHM HERE
        
        Inputs:
        board_state - array of what weight is at a given position on the board

        Output:
        position (Integer)
        """
        position = randint(-self.board_length, self.board_length)

        while board_state[str(position)] != 0:
            position = randint(-self.board_length, self.board_length)

        return position

    def copy_board_with_updated_move(self, old_board, position, weight):
        new_board = old_board.copy()
        new_board[position] = weight
        return new_board

    def check_balance(self, board):
        left_torque = 0
        right_torque = 0
        for i in range(0,61):
            left_torque += (i - 30 + 3) * board[i]
            right_torque += (i - 30 + 1) * board[i]
        left_torque += 3 * 3
        right_torque += 1 * 3
        return left_torque >= 0 and right_torque <= 0


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--first', action='store_true', default=False, help='Indicates whether client should go first')
    parser.add_argument('--ip', type=str, default= 'localhost')
    parser.add_argument('--port', type=int, default= 3000)
    parser.add_argument('--name', type=str, default= "Lily")
    args = parser.parse_args()


    HOST = args.ip
    PORT = args.port

    player = NoTippingClient(args.name, args.first)
    player.play_game()













