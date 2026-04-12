#ifndef ISIGNALCUSTOMNAME_H
#define ISIGNALCUSTOMNAME_H

#include <QString>

class DcController;
class ParameterElement;

class ISignalCustomName
{
public:
    ISignalCustomName(uint16_t addr, uint16_t elementIdx);

    // Пользовательское имя
    QString customName() const;
    void setCustomName(const QString &name);

protected:
    bool init(DcController* config);
    ParameterElement* nameElement() const;

private:
    uint16_t m_addr;
    uint16_t m_elementIdx;
    ParameterElement* m_nameElement = nullptr;
};

#endif // ISIGNALCUSTOMNAME_H
