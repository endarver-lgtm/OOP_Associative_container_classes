#pragma once
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "EquipmentRecord.h"

class AssociativeManager {
private:
    // 1. MAP: Реестр техники (ключ - уникальный ID)
    std::map<int, EquipmentRecord> equipmentMap_;

    // 2. SET: Множество уникальных строительных объектов (без дубликатов, по алфавиту)
    std::set<std::string> sitesSet_;

    // 3. MULTIMAP: Распределение техники по объектам (ключ - стройобъект, может быть несколько единиц)
    std::multimap<std::string, EquipmentRecord> siteAssignments_;

    // 4. MULTISET: Сетка почасовых тарифов (допускает одинаковые тарифы, упорядочена)
    std::multiset<double> ratesMultiset_;

    // 5. UNORDERED_MAP: Хеш-таблица суммарной наработки часов по госномеру (доступ O(1))
    std::unordered_map<std::string, int> hoursByRegNumber_;

    // 6. UNORDERED_SET: Хеш-таблица уникальных категорий работ
    std::unordered_set<std::string> workCategories_;

    // 7. UNORDERED_MULTIMAP: Хеш-таблица привязки моделей к госномерам (модель -> несколько госномеров)
    std::unordered_multimap<std::string, std::string> modelsToRegNumbers_;

    // 8. UNORDERED_MULTISET: Хеш-таблица длительностей отработанных смен
    std::unordered_multiset<int> shiftDurations_;

    int nextId_;

public:
    AssociativeManager();

    void seedDemoData();

    // --- 1. Работа с MAP (Основной реестр техники) ---
    void addRecord(const std::string& model, const std::string& reg,
                   const std::string& site, double rate, int hours, const std::string& category);
    bool removeRecord(int id);
    bool editRecord(int id, double newRate, int newHours, const std::string& newSite);
    EquipmentRecord* findRecordById(int id);
    std::vector<EquipmentRecord> searchByModelOrReg(const std::string& query) const;
    std::vector<EquipmentRecord> filterByCategory(const std::string& cat) const;
    void printMap(std::ostream& os) const;
    void exportMapToFile(const std::string& filename) const;
    std::map<int, EquipmentRecord>& equipmentMap() { return equipmentMap_; }

    // --- 2. Работа с SET (Строительные площадки) ---
    void addSite(const std::string& site);
    bool removeSite(const std::string& site);
    bool hasSite(const std::string& site) const;
    void printSitesSet(std::ostream& os) const;

    // --- 3. Работа с MULTIMAP (Назначения по объектам) ---
    void printMultimap(std::ostream& os) const;
    std::vector<EquipmentRecord> getEquipmentOnSite(const std::string& site) const;
    void removeSiteFromMultimap(const std::string& site);

    // --- 4. Работа с MULTISET (Тарифная сетка и алгоритмы lower_bound, upper_bound, equal_range) ---
    void addRate(double rate);
    void printMultisetRates(std::ostream& os) const;
    void demoRateRange(double key, std::ostream& os) const;

    // --- 5. Работа с UNORDERED_MAP (Наработка по госномеру) ---
    void printUnorderedMapHours(std::ostream& os) const;
    void addHours(const std::string& regNumber, int extraHours);

    // --- 6. Работа с UNORDERED_SET (Категории работ) ---
    void printUnorderedSetCategories(std::ostream& os) const;

    // --- 7. Работа с UNORDERED_MULTIMAP (Модели и их госномера) ---
    void printUnorderedMultimapModels(std::ostream& os) const;

    // --- 8. Работа с UNORDERED_MULTISET (Длительности смен) ---
    void printUnorderedMultisetShifts(std::ostream& os) const;

    // --- Сводный вывод и экспорт всех 8 контейнеров ---
    void printAllContainers(std::ostream& os) const;
    void exportAllToFile(const std::string& filename) const;
};
