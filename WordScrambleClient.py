#!/usr/bin/python

import socket
import struct
from kivy.app import App
from kivy.uix.button import Button
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.clock import Clock
import NetworkingConstants

host_name = ''
port_number = ''
try:
    with open('config', 'r') as f:
        line1 = f.readline().strip()
        line2 = f.readline().strip()

        if not line1 or not line2:
            raise ValueError("File is empty")

        host_name = line1
        port_number = int(line2)

except FileNotFoundError:
    print("File not found")
except ValueError as e:
    print(e)
finally:
    f.close()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host_name, port_number))
sock.settimeout(3)

playerName = ''
initialGameState = "X Y Z"


def close_app(instance):
    # TERMINATE CONNECTION
    sock.close()

    # CLOSE APP
    App.get_running_app().stop()


def rec_message_size(soc):
    message_size = soc.recv(2)
    message_size = struct.unpack("H", message_size)[0]
    message_size = socket.ntohs(message_size)
    return message_size


def rec_message(soc):
    message = b''
    message_size = rec_message_size(soc)
    while len(message) < message_size:
        buf = soc.recv(min(message_size - len(message), 1024))
        if not buf:
            break
        message += buf
    # Message gets split into type (head) and arguments (tail)
    split_message = message.decode().split()
    if len(split_message) > 1:
        return split_message[0], split_message[1:]
    else:
        return split_message[0], []


def send_message(soc, msg):
    msg = msg.encode()
    msg_size = len(msg)
    msg_size = struct.pack("H", socket.htons(msg_size))
    soc.send(msg_size)
    # soc.sendall(msg)
    # soc.sendall(packed_msg_size + msg)
    sent = 0
    while sent < len(msg):
        buf = min(1024, len(msg) - sent)
        sent += soc.send(msg[sent:sent+buf])


class LoginScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        global sock

        self.layout = BoxLayout(orientation='vertical')

        self.welcome_label = Label(text='Welcome to Word Scramble')
        self.layout.add_widget(self.welcome_label)

        self.username_input = TextInput(text='Username')
        self.password_input = TextInput(text='Password')
        self.login_button = Button(text='Log In')
        self.login_button.bind(on_press=self.log_in)
        self.layout.add_widget(self.username_input)
        self.layout.add_widget(self.password_input)
        self.layout.add_widget(self.login_button)

        self.queue_button = Button(text='Join Queue')
        self.queue_button.bind(on_press=self.queue_for_game)
        self.queue_button.disabled = True
        self.layout.add_widget(self.queue_button)

        self.logout_button = Button(text='Log Out', disabled=True)
        self.logout_button.bind(on_press=self.log_out)
        self.layout.add_widget(self.logout_button)

        self.message_label = Label()
        self.layout.add_widget(self.message_label)

        self.close_app_button = Button(text='Close App')
        self.close_app_button.bind(on_press=close_app)
        self.layout.add_widget(self.close_app_button)

        self.add_widget(self.layout)

    def on_enter(self):
        sock.settimeout(3)

    def log_in(self, instance):
        # called by a Button press

        # collect and send credentials
        message = str(NetworkingConstants.ClientInstructionType.logIn.value) + " " + self.username_input.text + " " + self.password_input.text
        send_message(sock, message)

        # wait for response
        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError:
            #   pass
            print("Timeout while Logging In")
        else:
            if type == str(NetworkingConstants.ServerInstructionType.logInResult.value):
                if answer[0] == str(NetworkingConstants.LogInResultType.failureUser.value):
                    self.message_label.text = 'Incorrect username'
                elif answer[0] == str(NetworkingConstants.LogInResultType.failureLoggedIn.value):
                    self.message_label.text = 'User already logged in'
                elif answer[0] == str(NetworkingConstants.LogInResultType.failurePassword.value):
                    self.message_label.text = 'Incorrect password'
                elif answer[0] == str(NetworkingConstants.LogInResultType.success.value) and answer[1] == self.username_input.text:
                    self.message_label.text = 'Logged in'
                    global playerName
                    playerName = self.username_input.text
                    self.queue_button.disabled = False
                    self.login_button.disabled = True
                    self.logout_button.disabled = False


    def queue_for_game(self, instance):
        # called on login

        message = str(NetworkingConstants.ClientInstructionType.logOut.queueForGame.value)
        send_message(sock, message)
        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError :
            #   pass
            print("Timeout while joining a game")
        else:
            if type == str(NetworkingConstants.ServerInstructionType.addedToQueue.value):
                if answer[0] == str(NetworkingConstants.GenericResponseType.genericFailure.value):
                    print("Error while joining queue")
                elif answer[0] == str(NetworkingConstants.GenericResponseType.genericSuccess.value):
                    self.manager.current = 'queue'
                else:
                    # space for potential future error handling
                    pass

    def log_out(self, instance):
        message = str(NetworkingConstants.ClientInstructionType.logOut.value)
        send_message(sock, message)
        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError :
            #   pass
            print("Timeout while Logging Out")
        else:
            if type == str(NetworkingConstants.ServerInstructionType.logOutResult.value):
                if answer[0] == str(NetworkingConstants.LogOutResultType.logOutSuccess.value):
                    self.message_label.text = 'Logged out'
                    global playerName
                    playerName = ''
                    self.manager.current = 'login'
                    self.login_button.disabled = False
                    self.logout_button.disabled = True
                    self.queue_button.disabled = True
                elif answer[0] == str(NetworkingConstants.LogOutResultType.logOutFailureUser.value):
                    print("Error while logging out")
                elif answer[0] == str(NetworkingConstants.LogOutResultType.logOutFailureLoggedOut.value):
                    print("Error while logging out (already logged out)")
                else:
                    # space for potential future error handling
                    pass


class QueueScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        global sock

        self.layout = BoxLayout(orientation='vertical')

        self.leave_queue_button = Button(text='Leave Queue')
        self.leave_queue_button.bind(on_press=self.leave_queue)
        self.layout.add_widget(self.leave_queue_button)

        self.add_widget(self.layout)

    def on_enter(self):
        Clock.schedule_once(self.check_answer, 1)
        sock.settimeout(0.25)

    def check_answer(self, dt):

        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError :
            #   pass
            print("Waiting for an oponent...")
            Clock.schedule_once(self.check_answer, 1)
        else:
            if type == str(NetworkingConstants.ServerInstructionType.gameReady.value):
                global initialGameState
                global playerName
                initialGameState =  "{0} {1} {2}".format(playerName, answer[0], answer[1])
                self.manager.current = 'lobby'
            elif type == str(NetworkingConstants.ServerInstructionType.removedFromQueue.value) and answer[0] == str(NetworkingConstants.GenericResponseType.genericSuccess.value):
                self.manager.current = 'login'
            else:
                Clock.schedule_once(self.check_answer, 1)

    def leave_queue(self, instance):
        Clock.unschedule(self.check_answer)
        message = str(NetworkingConstants.ClientInstructionType.leaveQueue.value)
        send_message(sock, message)
        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError :
            #   pass
            print("Timeout")
        else:
            if type == str(NetworkingConstants.ServerInstructionType.removedFromQueue.value) and answer[0] == str(NetworkingConstants.GenericResponseType.genericSuccess.value):
                self.manager.current = 'login'


class LobbyScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        global sock
        global playerName
        global initialGameState

        enemy_name = initialGameState.split()[1]

        self.layout = BoxLayout(orientation='vertical')

        self.match_label = Label(text=playerName + " vs " + enemy_name)
        self.layout.add_widget(self.match_label)

        self.letters_label = Label(text=initialGameState.split()[2])
        self.result_label = Label(text='')
        self.word_input = TextInput(text='', multiline=False)
        self.word_button = Button(text='Send Word')
        self.word_button.bind(on_press=self.send_word)
        self.layout.add_widget(self.letters_label)
        self.layout.add_widget(self.result_label)
        self.layout.add_widget(self.word_input)
        self.layout.add_widget(self.word_button)

        self.add_widget(self.layout)

    def on_enter(self):
        enemy_name = initialGameState.split()[1]

        self.match_label.text = "{0} vs {1}".format(playerName, enemy_name)

        self.letters_label.text = initialGameState.split()[2]
        Clock.schedule_once(self.check_answer, 1)

    def check_answer(self, dt):
        try:
            type, answer = rec_message(sock)
        except Exception as e:
            # if e == TimeoutError:
            #   print("Timeout")
            #   pass
            Clock.schedule_once(self.check_answer, 1)
        else:
            if type == str(NetworkingConstants.ServerInstructionType.gameOver.value) and answer[0] == str(NetworkingConstants.GameOverParams.gameWon.value):
                #self.manager.get_screen("result").set_game_result("You Won!")
                result_screen_ref = self.manager.get_screen("result")
                result_screen_ref.set_game_result("You Won!")
                self.manager.current = 'result'
            elif type == str(NetworkingConstants.ServerInstructionType.gameOver.value) and answer[0] == str(NetworkingConstants.GameOverParams.gameLost.value):
                self.manager.get_screen('result').set_game_result("You Lost!")
                self.manager.current = 'result'
            elif type == str(NetworkingConstants.ServerInstructionType.playerAdvancedRound.value):
                self.result_label.text = 'You scored!'
                self.word_input.text = ''
                self.letters_label.text = answer[0]
                Clock.schedule_once(self.check_answer, 1)
            elif type == str(NetworkingConstants.ServerInstructionType.enemyAdvancedRound.value):
                self.result_label.text = 'Enemy scored!'
                # Placeholder in case we wanted enemy advancing round to trigger some other effect
                Clock.schedule_once(self.check_answer, 1)
            elif type == str(NetworkingConstants.ServerInstructionType.answerRejected.value):
                self.result_label.text = 'Incorrect answer!'
                Clock.schedule_once(self.check_answer, 1)
            else:
                Clock.schedule_once(self.check_answer, 1)

    def send_word(self, instance):
        message = str(NetworkingConstants.ClientInstructionType.sendWord.value) + " " + self.word_input.text
        send_message(sock, message)


class ResultScreen(Screen):
    game_result = ''
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.layout = BoxLayout(orientation='vertical')

        self.result_label = Label(text=self.game_result)
        self.layout.add_widget(self.result_label)

        self.leave_game_button = Button(text='Back to Login Screen')
        self.leave_game_button.bind(on_press=self.main_menu)
        self.layout.add_widget(self.leave_game_button)

        self.add_widget(self.layout)

    def main_menu(self, instance):
        self.manager.current = 'login'
        self.set_game_result('')

    def set_game_result(self, text):
        self.game_result = text
        self.result_label.text = self.game_result


class WordScrambleClient(App):
    def __init__(self):
        super().__init__()

        self.screen_manager = ScreenManager()

        self.login_screen = LoginScreen(name='login')
        self.queue_screen = QueueScreen(name='queue')
        self.lobby_screen = LobbyScreen(name='lobby')
        self.result_screen = ResultScreen(name='result')

    def build(self):
        self.screen_manager.add_widget(self.login_screen)
        self.screen_manager.add_widget(self.queue_screen)
        self.screen_manager.add_widget(self.lobby_screen)
        self.screen_manager.add_widget(self.result_screen)

        return self.screen_manager


if __name__ == '__main__':
    client = WordScrambleClient()
    client.run()
