#include "EquipmentRecord.h"
#include <iomanip>

EquipmentRecord::EquipmentRecord()
    : id_(0), model_("—"), regNumber_("—"), siteName_("—"),
      hourlyRate_(0.0), hoursWorked_(0), category_("—") {
}

EquipmentRecord::EquipmentRecord(int id, const std::string& model, const std::string& regNumber,
                                 const std::string& siteName, double hourlyRate, int hoursWorked,
                                 const std::string& category)
    : id_(id), model_(model), regNumber_(regNumber), siteName_(siteName),
      hourlyRate_(hourlyRate), hoursWorked_(hoursWorked), category_(category) {
}

void EquipmentRecord::printHeader(std::ostream& os) {
    std::ios::fmtflags old = os.flags();
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(5)  << "ID"
       << std::setw(22) << "Модель"
       << std::setw(14) << "Гос. номер"
       << std::setw(22) << "Стройобъект"
       << std::setw(14) << "Тариф, руб/ч"
       << std::setw(10) << "Отраб. ч"
       << std::setw(15) << "Сумма, руб."
       << std::setw(20) << "Категория работ"
       << "\n";
    os << std::string(122, '-') << "\n";
    os.flags(old);
}

void EquipmentRecord::printRow(std::ostream& os) const {
    std::ios::fmtflags old = os.flags();
    std::streamsize oldPrec = os.precision();

    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(5)  << id_
       << std::setw(22) << model_
       << std::setw(14) << regNumber_
       << std::setw(22) << siteName_;

    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setw(14) << std::setprecision(2) << hourlyRate_;

    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(10) << hoursWorked_;

    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setw(15) << std::setprecision(2) << totalCost();

    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(20) << category_
       << "\n";

    os.flags(old);
    os.precision(oldPrec);
}
