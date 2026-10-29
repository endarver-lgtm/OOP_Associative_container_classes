#include "AssociativeManager.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>

static std::string toLowerStr(std::string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

AssociativeManager::AssociativeManager()
    : nextId_(1) {
}

void AssociativeManager::seedDemoData() {
    // Начальное наполнение предметной области: "Учет занятости строительной техники"
    addRecord("Caterpillar 320D", "5432 AB-7", "ЖК 'Минск-Мир'",  130.00, 80,  "Земляные работы");
    addRecord("Komatsu PC210",    "1122 BK-7", "Мост через Немигу", 115.50, 50,  "Земляные работы");
    addRecord("Liebherr LTM 1050", "7700 TT-7", "ЖК 'Минск-Мир'",  180.00, 56,  "Монтажные работы");
    addRecord("Ивановец КС-45717", "3344 AX-7", "ТЦ 'Северный'",   95.00,  40,  "Погрузочные работы");
    addRecord("JCB JS220",        "8899 MH-7", "ТЦ 'Северный'",   125.00, 60,  "Земляные работы");
    addRecord("Zoomlion ZTC600",   "9012 EE-7", "Стадион 'Динамо'",210.00, 32,  "Монтажные работы");
    addRecord("Caterpillar 320D", "6543 AB-7", "Стадион 'Динамо'",130.00, 48,  "Земляные работы"); // Повтор модели и тарифа для демонстрации multiset и unordered_multimap
}

// =================== 1. MAP (Реестр техники) ===================

void AssociativeManager::addRecord(const std::string& model, const std::string& reg,
                                   const std::string& site, double rate, int hours,
                                   const std::string& category) {
    int id = nextId_++;
    EquipmentRecord rec(id, model, reg, site, rate, hours, category);

    // 1. Вставка в map
    equipmentMap_.insert(std::make_pair(id, rec));

    // 2. Вставка стройобъекта в set
    sitesSet_.insert(site);

    // 3. Вставка в multimap (привязка объекта к технике)
    siteAssignments_.insert(std::make_pair(site, rec));

    // 4. Вставка тарифа в multiset
    ratesMultiset_.insert(rate);

    // 5. Обновление наработки в unordered_map
    hoursByRegNumber_[reg] += hours;

    // 6. Вставка категории в unordered_set
    workCategories_.insert(category);

    // 7. Вставка в unordered_multimap (модель -> госномер)
    modelsToRegNumbers_.insert(std::make_pair(model, reg));

    // 8. Вставка длительности смены в unordered_multiset
    shiftDurations_.insert(hours);
}

bool AssociativeManager::removeRecord(int id) {
    auto it = equipmentMap_.find(id);
    if (it == equipmentMap_.end()) return false;

    std::string site = it->second.siteName();
    std::string reg = it->second.regNumber();
    double rate = it->second.hourlyRate();

    // Удаляем из map
    equipmentMap_.erase(it);

    // Удаляем из multimap
    auto range = siteAssignments_.equal_range(site);
    for (auto mit = range.first; mit != range.second; ) {
        if (mit->second.id() == id) mit = siteAssignments_.erase(mit);
        else ++mit;
    }

    // Удаляем один тариф из multiset
    auto rateIt = ratesMultiset_.find(rate);
    if (rateIt != ratesMultiset_.end()) ratesMultiset_.erase(rateIt);

    return true;
}

bool AssociativeManager::editRecord(int id, double newRate, int newHours, const std::string& newSite) {
    auto it = equipmentMap_.find(id);
    if (it == equipmentMap_.end()) return false;

    // Обновляем в multiset
    auto rateIt = ratesMultiset_.find(it->second.hourlyRate());
    if (rateIt != ratesMultiset_.end()) ratesMultiset_.erase(rateIt);
    ratesMultiset_.insert(newRate);

    // Обновляем в map
    it->second.setHourlyRate(newRate);
    it->second.setHoursWorked(newHours);
    it->second.setSiteName(newSite);
    sitesSet_.insert(newSite);

    // Перестраиваем назначение в multimap
    std::string oldSite = it->second.siteName();
    auto range = siteAssignments_.equal_range(oldSite);
    for (auto mit = range.first; mit != range.second; ) {
        if (mit->second.id() == id) mit = siteAssignments_.erase(mit);
        else ++mit;
    }
    siteAssignments_.insert(std::make_pair(newSite, it->second));

    return true;
}

EquipmentRecord* AssociativeManager::findRecordById(int id) {
    auto it = equipmentMap_.find(id);
    if (it != equipmentMap_.end()) return &(it->second);
    return nullptr;
}

std::vector<EquipmentRecord> AssociativeManager::searchByModelOrReg(const std::string& query) const {
    std::vector<EquipmentRecord> res;
    std::string q = toLowerStr(query);
    for (const auto& pair : equipmentMap_) {
        if (toLowerStr(pair.second.model()).find(q) != std::string::npos ||
            toLowerStr(pair.second.regNumber()).find(q) != std::string::npos) {
            res.push_back(pair.second);
        }
    }
    return res;
}

std::vector<EquipmentRecord> AssociativeManager::filterByCategory(const std::string& cat) const {
    std::vector<EquipmentRecord> res;
    std::string c = toLowerStr(cat);
    for (const auto& pair : equipmentMap_) {
        if (toLowerStr(pair.second.category()).find(c) != std::string::npos) {
            res.push_back(pair.second);
        }
    }
    return res;
}

void AssociativeManager::printMap(std::ostream& os) const {
    os << "\n=== 1. РЕЕСТР ТЕХНИКИ (std::map<int, EquipmentRecord>, записей: " << equipmentMap_.size() << ") ===\n";
    if (equipmentMap_.empty()) {
        os << "(реестр пуст)\n";
        return;
    }
    EquipmentRecord::printHeader(os);
    for (const auto& pair : equipmentMap_) {
        pair.second.printRow(os);
    }
}

void AssociativeManager::exportMapToFile(const std::string& filename) const {
    std::ofstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи\n";
        return;
    }
    printMap(f);
    std::cout << "Данные реестра (map) успешно сохранены в файл: " << filename << "\n";
}

// =================== 2. SET (Строительные объекты) ===================

void AssociativeManager::addSite(const std::string& site) {
    sitesSet_.insert(site);
}

bool AssociativeManager::removeSite(const std::string& site) {
    return sitesSet_.erase(site) > 0;
}

bool AssociativeManager::hasSite(const std::string& site) const {
    return sitesSet_.find(site) != sitesSet_.end();
}

void AssociativeManager::printSitesSet(std::ostream& os) const {
    os << "\n=== 2. СТРОИТЕЛЬНЫЕ ОБЪЕКТЫ (std::set<std::string>, уникальных: " << sitesSet_.size() << ") ===\n";
    int i = 1;
    for (const auto& site : sitesSet_) {
        os << "  " << i++ << ") " << site << "\n";
    }
}

// =================== 3. MULTIMAP (Назначения по объектам) ===================

void AssociativeManager::printMultimap(std::ostream& os) const {
    os << "\n=== 3. РАСПРЕДЕЛЕНИЕ ТЕХНИКИ ПО ОБЪЕКТАМ (std::multimap<string, EquipmentRecord>) ===\n";
    if (siteAssignments_.empty()) {
        os << "(назначений нет)\n";
        return;
    }
    EquipmentRecord::printHeader(os);
    for (const auto& pair : siteAssignments_) {
        pair.second.printRow(os);
    }
}

std::vector<EquipmentRecord> AssociativeManager::getEquipmentOnSite(const std::string& site) const {
    std::vector<EquipmentRecord> res;
    auto range = siteAssignments_.equal_range(site);
    for (auto it = range.first; it != range.second; ++it) {
        res.push_back(it->second);
    }
    return res;
}

void AssociativeManager::removeSiteFromMultimap(const std::string& site) {
    siteAssignments_.erase(site);
}

// =================== 4. MULTISET (Тарифная сетка и алгоритмы) ===================

void AssociativeManager::addRate(double rate) {
    ratesMultiset_.insert(rate);
}

void AssociativeManager::printMultisetRates(std::ostream& os) const {
    os << "\n=== 4. ТАРИФНАЯ СЕТКА (std::multiset<double>, ставок: " << ratesMultiset_.size() << ") ===\n";
    os << "Ставки (руб/ч, отсортированы): ";
    for (double r : ratesMultiset_) {
        os << std::fixed << std::setprecision(2) << r << "  ";
    }
    os << "\n";
}

void AssociativeManager::demoRateRange(double key, std::ostream& os) const {
    os << "\n--- Демонстрация методов equal_range(), lower_bound(), upper_bound() по методичке ---\n";
    os << "Ищем тариф: " << key << " руб/ч\n";

    // 1. count()
    size_t cnt = ratesMultiset_.count(key);
    os << "1) Метод count(" << key << "): найдено " << cnt << " совпадений.\n";

    // 2. equal_range()
    auto range = ratesMultiset_.equal_range(key);
    if (range.first != ratesMultiset_.end()) {
        os << "2) Метод equal_range(" << key << "):\n"
           << "   - первый элемент в диапазоне (*p.first) = " << *(range.first) << "\n";
        if (range.second != ratesMultiset_.end()) {
            os << "   - первый элемент за пределами (*p.second) = " << *(range.second) << "\n";
        } else {
            os << "   - p.second указывает на end()\n";
        }
    } else {
        os << "2) Тариф не найден в multiset.\n";
    }

    // 3. lower_bound() - первый элемент >= key
    auto lb = ratesMultiset_.lower_bound(key);
    if (lb != ratesMultiset_.end()) {
        os << "3) Метод lower_bound(" << key << ") [элемент >= " << key << "]: " << *lb << "\n";
    } else {
        os << "3) lower_bound: все элементы меньше " << key << "\n";
    }

    // 4. upper_bound() - первый элемент > key
    auto ub = ratesMultiset_.upper_bound(key);
    if (ub != ratesMultiset_.end()) {
        os << "4) Метод upper_bound(" << key << ") [элемент > " << key << "]: " << *ub << "\n";
    } else {
        os << "4) upper_bound: нет элементов больше " << key << "\n";
    }
}

// =================== 5. UNORDERED_MAP (Наработка) ===================

void AssociativeManager::printUnorderedMapHours(std::ostream& os) const {
    os << "\n=== 5. НАРАБОТКА ПО ГОСНОМЕРАМ (std::unordered_map<string, int>, хеш-таблица O(1)) ===\n";
    for (const auto& pair : hoursByRegNumber_) {
        os << "  Гос. номер: " << std::setw(12) << std::left << pair.first
           << " -> Суммарно отработано: " << pair.second << " ч.\n";
    }
}

void AssociativeManager::addHours(const std::string& regNumber, int extraHours) {
    hoursByRegNumber_[regNumber] += extraHours;
}

// =================== 6. UNORDERED_SET (Категории) ===================

void AssociativeManager::printUnorderedSetCategories(std::ostream& os) const {
    os << "\n=== 6. КАТЕГОРИИ РАБОТ (std::unordered_set<string>, хеш-множество O(1)) ===\n";
    for (const auto& cat : workCategories_) {
        os << "  • " << cat << "\n";
    }
}

// =================== 7. UNORDERED_MULTIMAP (Модели и госномера) ===================

void AssociativeManager::printUnorderedMultimapModels(std::ostream& os) const {
    os << "\n=== 7. ГРУППИРОВКА МАШИН ПО МОДЕЛЯМ (std::unordered_multimap<string, string>) ===\n";
    for (const auto& pair : modelsToRegNumbers_) {
        os << "  Модель: " << std::setw(22) << std::left << pair.first
           << " -> Гос. номер: " << pair.second << "\n";
    }
}

// =================== 8. UNORDERED_MULTISET (Длительности смен) ===================

void AssociativeManager::printUnorderedMultisetShifts(std::ostream& os) const {
    os << "\n=== 8. ДЛИТЕЛЬНОСТИ СМЕН (std::unordered_multiset<int>) ===\n";
    os << "Отработанные смены (часы): ";
    for (int h : shiftDurations_) {
        os << h << "ч  ";
    }
    os << "\n";
}

// =================== Сводный вывод и экспорт ===================

void AssociativeManager::printAllContainers(std::ostream& os) const {
    os << "===================================================================================\n";
    os << "   ДЕМОНСТРАЦИЯ ВСЕХ 8 АССОЦИАТИВНЫХ КОНТЕЙНЕРОВ STL (ЛР №4, Вариант 10)           \n";
    os << "===================================================================================\n";
    printMap(os);
    printSitesSet(os);
    printMultimap(os);
    printMultisetRates(os);
    printUnorderedMapHours(os);
    printUnorderedSetCategories(os);
    printUnorderedMultimapModels(os);
    printUnorderedMultisetShifts(os);
}

void AssociativeManager::exportAllToFile(const std::string& filename) const {
    std::ofstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи\n";
        return;
    }
    printAllContainers(f);
    std::cout << "Все 8 ассоциативных контейнеров успешно экспортированы в файл: " << filename << "\n";
}
