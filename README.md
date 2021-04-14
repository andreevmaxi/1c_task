1C_task
-----
### Описание задачи
Код задачи: 142

Реализуйте механизм, позволяющий хранить изменения между двумя бинарными файлами.

На вход операции "Рассчитать диф" подаются два бинарных файла ("старый" и "новый"). Операция должна оптимально сформировать файл разницы между ними.

На вход операции "Восстановить" подаётся "старый" файл и файл разницы. Она должна восстановить "новый" файл.

"Файлы" должны храниться и создаваться на диске, хранение промежуточных данных в оперативной памяти запрещено.

### Реализация
1.Функция calculate_diff(...) вычисляет разницу текстов при помощи алгоритма Вагнера — Фишера. Стоимости операций в динамике — 7, 8, 8 (удаление, вставка символа, замена) обоснованы желанием минимизировать объем промежуточного файла (очевидно, что кодирование операции удаления весит меньше, нежели остальных операций).

2.Функция update_file(...) восстанавливет новый текст из старого, при помощи закодированных операций в промежуточном файле

### Запуск
gcc main.cpp -o main
./main
