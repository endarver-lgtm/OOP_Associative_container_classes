#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "AssociativeManager.h"
#include "Input.h"

using namespace std;

// Подменю для основного учета (std::map)
static void menuMapOperations(AssociativeManager& mgr) {
    while (true) {
        clearScreen();
        cout << "--- УЧЕТ СТРОИТЕЛЬНОЙ ТЕХНИКИ (std::map<int, EquipmentRecord>) ---\n"
             << "1. Просмотр реестра техники на экран\n"
             << "2. Экспорт реестра в файл\n"
             << "3. Добавить запись о технике\n"
             << "4. Редактировать запись (тариф, часы, объект)\n"
             << "5. Удалить технику по ID\n"
             << "6. Поиск техники (по модели или госномеру)\n"
             << "7. Фильтр по категории работ\n"
             << "0. Назад в главное меню\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 7);
        clearScreen();

        if (choice == 0) break;

        switch (choice) {
        case 1:
            mgr.printMap(cout);
            break;
        case 2: {
            string filename = readLine(cin, "Введите имя файла (напр. registry.txt): ", 50);
            mgr.exportMapToFile(filename);
            break;
        }
        case 3: {
            cout << "Добавление записи о строительной технике:\n";
            string model = readLine(cin, "Модель техники (напр. Volvo EC220): ", 30);
            string reg = readLine(cin, "Гос. номер (напр. 1928 BK-7): ", 20);
            string site = readLine(cin, "Строительный объект (напр. ЖК Минск-Мир): ", 40);
            double rate = readDouble(cin, "Тариф, руб/час (1-9999): ", 1.0, 9999.0);
            int hours = readInt(cin, "Отработано часов (1-1000): ", 1, 1000);
            cout << "Выберите категорию работ:\n 1 - Земляные работы\n 2 - Монтажные работы\n 3 - Погрузочные работы\n";
            int catChoice = readInt(cin, "Категория: ", 1, 3);
            string cat = (catChoice == 1) ? "Земляные работы" : (catChoice == 2 ? "Монтажные работы" : "Погрузочные работы");

            mgr.addRecord(model, reg, site, rate, hours, cat);
            cout << "Запись успешно добавлена во все связанные ассоциативные контейнеры!\n";
            break;
        }
        case 4: {
            mgr.printMap(cout);
            int id = readInt(cin, "Введите ID записи для редактирования: ", 1, 9999);
            EquipmentRecord* rec = mgr.findRecordById(id);
            if (!rec) {
                cout << "Запись с таким ID не найдена.\n";
                break;
            }
            cout << "Текущий тариф: " << rec->hourlyRate() << " руб/ч. Новый тариф: ";
            double newRate = readDouble(cin, "", 1.0, 9999.0);
            cout << "Текущие часы: " << rec->hoursWorked() << " ч. Новые часы: ";
            int newHours = readInt(cin, "", 1, 1000);
            cout << "Текущий объект: " << rec->siteName() << ". Новый объект: ";
            string newSite = readLine(cin, "", 40);

            if (mgr.editRecord(id, newRate, newHours, newSite)) {
                cout << "Запись успешно обновлена!\n";
            }
            break;
        }
        case 5: {
            mgr.printMap(cout);
            int id = readInt(cin, "Введите ID для удаления: ", 1, 9999);
            if (askYesNo(cin, "Удалить эту технику из реестра? (y/n): ")) {
                if (mgr.removeRecord(id)) cout << "Запись успешно удалена.\n";
                else cout << "Запись с таким ID не найдена.\n";
            }
            break;
        }
        case 6: {
            string q = readLine(cin, "Введите часть модели или госномера: ", 30);
            auto found = mgr.searchByModelOrReg(q);
            if (found.empty()) {
                cout << "Ничего не найдено по запросу \"" << q << "\".\n";
            } else {
                cout << "\nНайденные единицы техники (" << found.size() << " ед.):\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : found) r.printRow(cout);
            }
            break;
        }
        case 7: {
            cout << "1 - Земляные работы\n2 - Монтажные работы\n3 - Погрузочные работы\n";
            int c = readInt(cin, "Категория: ", 1, 3);
            string cat = (c == 1) ? "Земляные работы" : (c == 2 ? "Монтажные работы" : "Погрузочные работы");
            auto filtered = mgr.filterByCategory(cat);
            cout << "\nРезультаты фильтра (" << filtered.size() << " ед.):\n";
            EquipmentRecord::printHeader(cout);
            for (const auto& r : filtered) r.printRow(cout);
            break;
        }
        }

        pauseScreen();
    }
}

// Меню для std::set и std::multimap
static void menuSitesAndMultimap(AssociativeManager& mgr) {
    while (true) {
        clearScreen();
        cout << "--- СТРОЙОБЪЕКТЫ (std::set) И НАЗНАЧЕНИЯ (std::multimap) ---\n"
             << "1. Просмотр списка уникальных стройобъектов (std::set)\n"
             << "2. Добавить новый строительный объект в set\n"
             << "3. Удалить объект из set\n"
             << "4. Просмотр распределения техники по объектам (std::multimap)\n"
             << "5. Поиск всей техники на выбранном объекте (equal_range в multimap)\n"
             << "0. Назад в главное меню\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 5);
        clearScreen();

        if (choice == 0) break;

        switch (choice) {
        case 1:
            mgr.printSitesSet(cout);
            break;
        case 2: {
            string site = readLine(cin, "Введите название строительного объекта: ", 40);
            mgr.addSite(site);
            cout << "Объект успешно добавлен в std::set (дубликаты автоматически исключаются)!\n";
            break;
        }
        case 3: {
            mgr.printSitesSet(cout);
            string site = readLine(cin, "Введите точное название объекта для удаления: ", 40);
            if (mgr.removeSite(site)) {
                cout << "Объект удален из множества set.\n";
            } else {
                cout << "Объект не найден в set.\n";
            }
            break;
        }
        case 4:
            mgr.printMultimap(cout);
            break;
        case 5: {
            mgr.printSitesSet(cout);
            string site = readLine(cin, "Введите название объекта для поиска техники: ", 40);
            auto list = mgr.getEquipmentOnSite(site);
            if (list.empty()) {
                cout << "На объекте \"" << site << "\" техника в данный момент не задействована.\n";
            } else {
                cout << "\nТехника на объекте \"" << site << "\" (найдено через equal_range: " << list.size() << " ед.):\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : list) r.printRow(cout);
            }
            break;
        }
        }

        pauseScreen();
    }
}

// Меню демонстрации алгоритмов multiset
static void menuMultisetDemo(AssociativeManager& mgr) {
    while (true) {
        clearScreen();
        cout << "--- ТАРИФНАЯ СЕТКА И АЛГОРИТМЫ (std::multiset<double>) ---\n"
             << "1. Вывод всех тарифов в multiset (отсортированы автоматически)\n"
             << "2. Добавить новый тариф в сетку\n"
             << "3. Демонстрация методов equal_range(), lower_bound(), upper_bound()\n"
             << "0. Назад в главное меню\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 3);
        clearScreen();

        if (choice == 0) break;

        switch (choice) {
        case 1:
            mgr.printMultisetRates(cout);
            break;
        case 2: {
            double rate = readDouble(cin, "Введите новый тариф (руб/ч): ", 1.0, 9999.0);
            mgr.addRate(rate);
            cout << "Тариф добавлен в multiset!\n";
            mgr.printMultisetRates(cout);
            break;
        }
        case 3: {
            mgr.printMultisetRates(cout);
            double key = readDouble(cin, "Введите значение тарифа для поиска границ: ", 1.0, 9999.0);
            mgr.demoRateRange(key, cout);
            break;
        }
        }

        pauseScreen();
    }
}

// Меню демонстрации хеш-контейнеров (unordered_*)
static void menuUnorderedContainers(AssociativeManager& mgr) {
    clearScreen();
    cout << "--- ДЕМОНСТРАЦИЯ ХЕШ-КОНТЕЙНЕРОВ (unordered_* - сложность O(1)) ---\n";
    mgr.printUnorderedMapHours(cout);
    mgr.printUnorderedSetCategories(cout);
    mgr.printUnorderedMultimapModels(cout);
    mgr.printUnorderedMultisetShifts(cout);
    pauseScreen();
}

int main() {
    // Настройка консоли на русский язык
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    AssociativeManager mgr;
    mgr.seedDemoData();

    while (true) {
        clearScreen();
        cout << "===================================================================================\n"
             << "   СИСТЕМА УЧЕТА СТРОИТЕЛЬНОЙ ТЕХНИКИ: АССОЦИАТИВНЫЕ КОНТЕЙНЕРЫ (ЛР №4, Вар. 10)   \n"
             << "===================================================================================\n"
             << "1. Сводный просмотр всех 8 ассоциативных контейнеров STL на экран\n"
             << "2. Экспорт всех данных в файл\n"
             << "3. Учет техники в реестре (контейнер std::map)\n"
             << "4. Стройобъекты и распределение (контейнеры std::set и std::multimap)\n"
             << "5. Тарифная сетка и алгоритмы lower_bound/upper_bound (std::multiset)\n"
             << "6. Просмотр хеш-контейнеров (unordered_map, unordered_set, unordered_multi*)\n"
             << "0. Выход из программы\n"
             << "-----------------------------------------------------------------------------------\n";

        int choice = readInt(cin, "Выберите пункт меню: ", 0, 6);
        clearScreen();

        switch (choice) {
        case 1:
            mgr.printAllContainers(cout);
            pauseScreen();
            break;
        case 2: {
            string filename = readLine(cin, "Введите имя файла (напр. all_associative.txt): ", 50);
            mgr.exportAllToFile(filename);
            pauseScreen();
            break;
        }
        case 3:
            menuMapOperations(mgr);
            break;
        case 4:
            menuSitesAndMultimap(mgr);
            break;
        case 5:
            menuMultisetDemo(mgr);
            break;
        case 6:
            menuUnorderedContainers(mgr);
            break;
        case 0:
            if (askYesNo(cin, "Вы действительно хотите выйти из программы? (y/n): ")) {
                cout << "\nПрограмма завершена. До свидания!\n";
                return 0;
            }
            break;
        }
    }

    return 0;
}
