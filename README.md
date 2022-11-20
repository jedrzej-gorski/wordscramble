# wordscramble

- Użytkownik loguje się na konto
- Wysyła do serwera chęć rozpoczęcia rozgrywki
- Gracz wstawiany jest do kolejki, aż powyższych dwóch kroków nie wykona co najmniej jeden inny użytkownik
- Rozgrywka rozpoczyna się po sparowaniu ze sobą dwóch graczy. 
- Gracze podczas aktywnej rozgrywki otrzymują zestaw znaków, z którego da się ułożyć co najmniej jedno słowo znajdujące się w zasobach serwera.
- Gracze biorą udział w pięciu rundach podczas jednej rozgrywki, podczas której układają słowa z podanych przez serwera liter. Serwer następnie weryfikuje poprawność słowa i przepuszcza gracza do następnej rundy (wysyłąjąc przy tym kolejny, *mniejszy* zestaw liter) bądź deklaruje gracza, jako zwycięzcę.
