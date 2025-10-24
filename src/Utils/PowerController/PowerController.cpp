/**
 * @file PowerController.cpp
 * @brief 電源管理クラスの実装
 */

#include "PowerController.hpp"

PowerController::PowerController() {}

void PowerController::begin() {
	LowPower.begin();
}

int PowerController::getVoltage() {
	return LowPower.getVoltage();
}

const char* PowerController::getBootCause() {
    bootcause_e cause = LowPower.bootCause();

    // ブート原因を人間可読な文字列に変換
    switch (cause) {
        // --- 通常起動 ---
        case POR_SUPPLY:      return "Power On (Supply)";
        case POR_NORMAL:      return "Power On (Normal)";

        // --- Watchdog による再起動/リセット ---
        case WDT_REBOOT:      return "Watchdog Reboot";
        case WDT_RESET:       return "Watchdog Reset";

        // --- DeepSleep からの復帰 ---
        // DeepSleep: CPU停止、メモリ保持、消費電力中程度
        case DEEP_WKUPL:      return "DeepSleep Wakeup (WKUPL)";
        case DEEP_WKUPS:      return "DeepSleep Wakeup (WKUPS)";
        case DEEP_RTC:        return "DeepSleep Wakeup (RTC Alarm)";
        case DEEP_USB_ATTACH: return "DeepSleep Wakeup (USB Attach)";
        case DEEP_OTHERS:     return "DeepSleep Wakeup (Others)";

        // --- ColdSleep からの復帰 ---
        // ColdSleep: CPU停止、メモリ非保持、消費電力最小
        case COLD_SCU_INT:    return "ColdSleep Wakeup (SCU INT)";
        case COLD_RTC_ALM0:   return "ColdSleep Wakeup (RTC ALM0)";
        case COLD_RTC_ALM1:   return "ColdSleep Wakeup (RTC ALM1)";
        case COLD_RTC_ALM2:   return "ColdSleep Wakeup (RTC ALM2)";
        case COLD_RTC_ALMERR: return "ColdSleep Wakeup (RTC ALMERR)";
        case COLD_SEN_INT:    return "ColdSleep Wakeup (SEN INT)";
        case COLD_PMIC_INT:   return "ColdSleep Wakeup (PMIC INT)";
        case COLD_USB_DETACH: return "ColdSleep Wakeup (USB Detach)";
        case COLD_USB_ATTACH: return "ColdSleep Wakeup (USB Attach)";

        // --- GPIO 割り込みによる復帰（IRQ36〜47） ---
        case COLD_GPIO_IRQ36: return "ColdSleep Wakeup (GPIO IRQ36)";
        case COLD_GPIO_IRQ37: return "ColdSleep Wakeup (GPIO IRQ37)";
        case COLD_GPIO_IRQ38: return "ColdSleep Wakeup (GPIO IRQ38)";
        case COLD_GPIO_IRQ39: return "ColdSleep Wakeup (GPIO IRQ39)";
        case COLD_GPIO_IRQ40: return "ColdSleep Wakeup (GPIO IRQ40)";
        case COLD_GPIO_IRQ41: return "ColdSleep Wakeup (GPIO IRQ41)";
        case COLD_GPIO_IRQ42: return "ColdSleep Wakeup (GPIO IRQ42)";
        case COLD_GPIO_IRQ43: return "ColdSleep Wakeup (GPIO IRQ43)";
        case COLD_GPIO_IRQ44: return "ColdSleep Wakeup (GPIO IRQ44)";
        case COLD_GPIO_IRQ45: return "ColdSleep Wakeup (GPIO IRQ45)";
        case COLD_GPIO_IRQ46: return "ColdSleep Wakeup (GPIO IRQ46)";
        case COLD_GPIO_IRQ47: return "ColdSleep Wakeup (GPIO IRQ47)";

        default:              return "Unknown Boot Cause";
    }
}