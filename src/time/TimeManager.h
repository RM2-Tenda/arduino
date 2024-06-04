#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "actuators/Buzzer.h"
#include "actuators/Fan.h"
#include "sensors/DHTSensor.h"
#include "sensors/GasSensor.h"
#include "sensors/PresenceSensor.h"
#include "sensors/UVSensor.h"
#include <Arduino.h>
#include <time.h>

#define MAX_ALARMS 10

/**
 * @struct Alarm
 * @brief Estrutura para armazenar informações de um alarme.
 */
struct Alarm {
    String sensor;
    String condition;
    String comparison;
    float value;
    String days;
    String startTime;
    String endTime;
};

/**
 * @class TimeManager
 * @brief Classe para gerenciar alarmes e tempo.
 */
class TimeManager {
  public:
    /**
     * @brief Inicializa o gerenciador de tempo.
     */
    void begin();

    /**
     * @brief Define o tempo atual.
     * @param timeString String representando o tempo atual.
     */
    void setTime(const String &timeString);

    /**
     * @brief Define um alarme.
     * @param sensor Sensor associado ao alarme.
     * @param condition Condição do alarme.
     * @param comparison Comparação do alarme.
     * @param value Valor do alarme.
     * @param days Dias em que o alarme está ativo.
     * @param startTime Hora de início do alarme.
     * @param endTime Hora de término do alarme.
     */
    void setAlarm(const String &sensor, const String &condition,
                  const String &comparison, float value, const String &days,
                  const String &startTime, const String &endTime);

    /**
     * @brief Verifica e aciona alarmes.
     * @param buzzer Referência ao objeto Buzzer.
     * @param fan Referência ao objeto Fan.
     * @param dht Referência ao objeto DHTSensor.
     * @param gas Referência ao objeto GasSensor.
     * @param uv Referência ao objeto UVSensor.
     * @param presence Referência ao objeto PresenceSensor.
     */
    void checkAlarms(Buzzer &buzzer, Fan &fan, DHTSensor &dht, GasSensor &gas,
                     UVSensor &uv, PresenceSensor &presence);

    /**
     * @brief Verifica se algum alarme está acionado.
     * @return true se algum alarme está acionado, false caso contrário.
     */
    bool isAnyAlarmTriggered() const;

  private:
    Alarm alarms[MAX_ALARMS]; ///< Array de alarmes.
    int alarmCount;           ///< Contador de alarmes.
    time_t currentTime;       ///< Tempo atual.
    bool isCurrentTimeInRange(const String &startTime, const String &endTime);
    bool isCurrentDayInRange(const String &days);
    bool anyAlarmTriggered; ///< Flag indica se algum alarme está acionado.
};

#endif