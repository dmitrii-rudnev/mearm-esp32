meArm web interface with ESP32 MCU

Коллеги! В этом репозитории находится исходный код решения, описанного в публикации "Управление манипулятором meArm через web-интерфейс"

Ссылка на публикацию: https://habr.com/ru/company/ruvds/blog/574214/

Вид web-интерфейса приведен на рисунке ниже:

<img src="https://habrastorage.org/webt/os/6t/ql/os6tqltuh9yzmfzmkg9cmnbsdzy.jpeg" width="240" />

Решение работает на любой отладочной плате с ESP32-WROOM на борту. Для питания серводвигателей требуется дополнительный источник питания +5В, 1.5А.

Схема соединений:

<img src="https://habrastorage.org/webt/_l/up/3r/_lup3rqvvk2avoxrvb0epvgebxk.png" width="480" />

Решение требует настройки поддержки ESP32 в Arduino IDE и установки библиотек:

<a href="https://github.com/mimeindustries/mearm-brains-arduino">https://github.com/mimeindustries/mearm-brains-arduino</a>

<a href="https://github.com/RoboticsBrno/ServoESP32">https://github.com/RoboticsBrno/ServoESP32</a>




