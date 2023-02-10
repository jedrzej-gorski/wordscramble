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

host_name = 'localhost'
port_number = 5050

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host_name, port_number))
sock.settimeout(5)

playerName = ''
initialGameState = "X Y Z"
mainScreenMessage = ''


def close_app(instance):
    # TERMINATE CONNECTION
    sock.close()

    # CLOSE APP
    App.get_running_app().stop()


def rec_message_size(soc):
    message_size = soc.recv(2)
    message_size = struct.unpack("!H", message_size)[0]
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
    return message.decode()


def send_message(soc, msg):

    msg = msg.encode()
    msg_size = len(msg)
    msg_size = struct.pack("!H", socket.htons(msg_size))
    soc.send(msg_size)
    # soc.sendall(msg)
    # sck.sendall(packed_msg_size + msg)
    sent = 0
    while sent < len(msg):
        buf = min(1024, len(msg) - sent)
        sent += soc.send(msg[sent:sent+buf])


class LoginScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        global sock
        global mainScreenMessage

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

        self.message_label = Label(text=mainScreenMessage)
        self.layout.add_widget(self.message_label)

        self.close_app_button = Button(text='Close App')
        self.close_app_button.bind(on_press=close_app)
        self.layout.add_widget(self.close_app_button)

        self.add_widget(self.layout)

    def log_in(self, instance):
        # called by a Button press

        # collect and send credentials
        message = str(NetworkingConstants.ClientInstructionType.logIn.value) + " " + self.username_input.text + " " + self.password_input.text
        send_message(sock, message)

        # wait for response
        try:
            answer = rec_message(sock)

        except Exception as e:
            print(str(e))
        else:
            if answer == str(NetworkingConstants.LogInResultType.failureUser.value):
                self.message_label.text = 'Incorrect username'
            elif answer == str(NetworkingConstants.LogInResultType.failureLoggedIn.value):
                self.message_label.text = 'User already logged in'
            elif answer == str(NetworkingConstants.LogInResultType.failurePassword.value):
                self.message_label.text = 'Incorrect password'
            elif answer == str(NetworkingConstants.LogInResultType.success.value) + " " + self.username_input.text:
                self.message_label.text = 'Logged in'
                global playerName
                playerName = self.username_input.text
                self.queue_button.disabled = False
                self.login_button.disabled = True
                self.logout_button.disabled = False

    def queue_for_game(self, instance):
        # called on login

        message = str(NetworkingConstants.ClientInstructionType.logOut.queueForGame)
        sock.sendall(message.encode())
        try:
            answer = str(sock.recv(1024).decode())
        except TimeoutError:
            print("Timeout")
        else:
            if answer == str(NetworkingConstants.ServerInstructionType.addedToQueue) + " " + str(NetworkingConstants.GenericResponseType.genericFailure.value):
                print("Error while joining queue")
            elif answer == str(NetworkingConstants.ServerInstructionType.addedToQueue) + " " + str(NetworkingConstants.GenericResponseType.genericSuccess.value):
                self.manager.current = 'queue'
            else:
                # space for potential future error handling
                pass

    def log_out(self, instance):
        message = str(NetworkingConstants.ClientInstructionType.logOut.value)
        sock.sendall(message.encode())
        try:
            answer = str(sock.recv(1024).decode())
        except TimeoutError:
            print("Timeout")
        else:
            if answer == str(NetworkingConstants.LogOutResultType.logOutSuccess.value):
                global playerName
                playerName = ''
                self.manager.current = 'login'
                self.login_button.disabled = False
                self.logout_button.disabled = True
            elif answer == str(NetworkingConstants.LogOutResultType.logOutFailureUser.value):
                print("Error while logging out")
            elif answer == str(NetworkingConstants.LogOutResultType.logOutFailureLoggedOut.value):
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

    def check_answer(self, dt):

        try:
            answer = str(sock.recv(1024).decode())
        except TimeoutError:
            print("Timeout")
        else:
            if answer.startswith(str(NetworkingConstants.ServerInstructionType.gameReady.value)):
                global initialGameState
                initialGameState = answer
                self.manager.current = 'lobby'
            elif answer == str(NetworkingConstants.ServerInstructionType.removedFromQueue.value) + " " +  str(NetworkingConstants.GenericResponseType.genericSuccess.value):
                self.manager.current = 'login'
            else:
                Clock.schedule_once(self.check_answer, 1)

    def leave_queue(self, instance):
        message = str(NetworkingConstants.ClientInstructionType.leaveQueue.value)
        sock.sendall(message.encode())
        try:
            answer = str(sock.recv(1024).decode())
        except TimeoutError:
            print("Timeout")
        else:
            if answer == str(NetworkingConstants.ServerInstructionType.removedFromQueue.value) + " " + str(NetworkingConstants.GenericResponseType.genericSuccess.value):
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
        self.word_input = TextInput(text='')
        self.word_button = Button(text='Send Word')
        self.word_button.bind(on_press=self.send_word)
        self.layout.add_widget(self.letters_label)
        self.layout.add_widget(self.word_input)
        self.layout.add_widget(self.word_button)

        self.add_widget(self.layout)

    def on_enter(self):
        Clock.schedule_once(self.check_answer, 1)

    def check_answer(self, dt):
        global mainScreenMessage
        try:
            answer = str(sock.recv(1024).decode())
        except TimeoutError:
            print("Timeout")
        else:
            if answer == str(NetworkingConstants.ServerInstructionType.gameOver.value) + " " + str(NetworkingConstants.GameOverParams.gameWon.value):
                mainScreenMessage = 'You Won!'
                self.message_label.text = mainScreenMessage
                self.manager.current = 'login'
            elif answer == str(NetworkingConstants.ServerInstructionType.gameOver.value) + " " + str(NetworkingConstants.GameOverParams.gameLost.value):
                mainScreenMessage = 'Enemy Won!'
                self.message_label.text = mainScreenMessage
                self.manager.current = 'login'
            elif answer.startswith(str(NetworkingConstants.ServerInstructionType.playerAdvancedRound)):
                self.message_label.text = 'You scored!'
                self.letters_label = answer.split(str(NetworkingConstants.ServerInstructionType.playerAdvancedRound), 1)[-1]
                Clock.schedule_once(self.check_answer, 1)
            elif answer.startswith(str(NetworkingConstants.ServerInstructionType.enemyAdvancedRound)):
                self.message_label.text = 'Enemy scored!'
                self.letters_label = answer.split(str(NetworkingConstants.ServerInstructionType.enemyAdvancedRound), 1)[-1]
                Clock.schedule_once(self.check_answer, 1)
            elif answer == str(NetworkingConstants.ServerInstructionType.answerRejected):
                self.message_label.text = 'Incorrect answer!'
                Clock.schedule_once(self.check_answer, 1)
            else:
                Clock.schedule_once(self.check_answer, 1)

    def send_word(self, instance):
        message = str(NetworkingConstants.ClientInstructionType.sendWord.value) + " " + self.word_input.text
        sock.sendall(message.encode())


class WordScrambleClient(App):
    def __init__(self):
        super().__init__()

        self.screen_manager = ScreenManager()

        self.login_screen = LoginScreen(name='login')
        self.queue_screen = QueueScreen(name='queue')
        self.lobby_screen = LobbyScreen(name='lobby')

    def build(self):
        self.screen_manager.add_widget(self.login_screen)
        self.screen_manager.add_widget(self.queue_screen)
        self.screen_manager.add_widget(self.lobby_screen)

        return self.screen_manager


if __name__ == '__main__':
    client = WordScrambleClient()
    client.run()
