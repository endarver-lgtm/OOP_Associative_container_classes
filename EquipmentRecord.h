#pragma once
#include <iostream>
#include <string>

// Класс записи о занятости строительной техники
class EquipmentRecord {
private:
    int id_;
    std::string model_;
    std::string regNumber_;
    std::string siteName_;
    double hourlyRate_;
    int hoursWorked_;
    std::string category_;

public:
    EquipmentRecord();
    EquipmentRecord(int id, const std::string& model, const std::string& regNumber,
                    const std::string& siteName, double hourlyRate, int hoursWorked,
                    const std::string& category);

    int id() const { return id_; }
    const std::string& model() const { return model_; }
    const std::string& regNumber() const { return regNumber_; }
    const std::string& siteName() const { return siteName_; }
    double hourlyRate() const { return hourlyRate_; }
    int hoursWorked() const { return hoursWorked_; }
    const std::string& category() const { return category_; }

    void setId(int id) { id_ = id; }
    void setModel(const std::string& m) { model_ = m; }
    void setRegNumber(const std::string& reg) { regNumber_ = reg; }
    void setSiteName(const std::string& site) { siteName_ = site; }
    void setHourlyRate(double rate) { hourlyRate_ = rate; }
    void setHoursWorked(int hours) { hoursWorked_ = hours; }
    void setCategory(const std::string& cat) { category_ = cat; }

    double totalCost() const { return hourlyRate_ * hoursWorked_; }

    bool operator<(const EquipmentRecord& other) const { return id_ < other.id_; }
    bool operator==(const EquipmentRecord& other) const { return id_ == other.id_; }

    static void printHeader(std::ostream& os);
    void printRow(std::ostream& os) const;
};
