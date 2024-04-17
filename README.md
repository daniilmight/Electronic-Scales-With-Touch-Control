# Электронные весы с сенсорным управлением

Этот проект представляет собой программу на языке C для микроконтроллеров AVR, использующую графический дисплей (GLCD) и аналого-цифровой преобразователь (ADC) для измерения массы.

## Начало работы

### Требования

- Программатор для загрузки кода в микроконтроллер
- Микроконтроллер ATmega32
- Графический дисплей, совместимый с библиотекой glcd.h

### Подключение

- Подключите графический дисплей к портам микроконтроллера, как указано в коде инициализации (функция init).
- Подключите аналогово-цифровой преобразователь (ADC) для измерения массы.

### Настройка проекта

- В коде проекта можно настроить параметры, такие как используемые порты, константы и другие параметры инициализации.

## Структура проекта

- `main.c`: Главный файл программы.
- `glcd.h`: Библиотека для работы с графическим дисплеем.

## Использование

1. Соберите и загрузите проект в микроконтроллер.
2. Запустите микроконтроллер.
3. Текущая масса будет отображена на графическом дисплее.
4. Используйте виртуальные кнопки на дисплее для взаимодействия с проектом.

## Видео работы устройства

https://github.com/daniilmight/Electronic-Scales-With-Touch-Control/assets/71521420/3d0d3935-92b9-4658-9280-8f43a5815bde

## Использованные библиотеки

Для вывода изображения на экран использовалась библиотека - https://www.ofoghelec.com/download/sample/2698

## Примечания

- Обратитесь к комментариям в коде для более подробной информации о функциях и переменных.

