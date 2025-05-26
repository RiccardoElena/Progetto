package furhatos.app.newskill.data.locale

// import furhatos.locale.Locale as FurhatLocale
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.util.Language
import java.util.Locale
import java.util.ResourceBundle

object Localization {
    fun getLocalizedString(
        key: String,
        lang: Language,
    ): String {
        lastLanguage = lang
        if (DEBUG_MODE) println("Answering in ${lastLanguage.code}")
        return getLocalizedString(key)
    }

    fun getLocalizedString(key: String): String {
        if (DEBUG_MODE) println("Answering in ${localeMapper(lastLanguage)}")
        val bundle = ResourceBundle.getBundle("string", localeMapper(lastLanguage))
        return bundle.getString(key)
    }

    fun statelessGetLocalizedString(
        key: String,
        lang: Language,
    ): String {
        if (DEBUG_MODE) println("Stateless Answering in ${localeMapper(lang)}")
        val bundle = ResourceBundle.getBundle("string", localeMapper(lang))
        return bundle.getString(key)
    }

    private fun localeMapper(lang: Language): Locale =
        when (lang.code.lowercase()) {
            "fr-fr" -> Locale("fr", "FR")
            "es-es" -> Locale("es", "ES")
            "en-gb" -> Locale("en", "EN")
            else -> Locale("default", "Default")
        }

    var lastLanguage: Language = Language.ITALIAN_CH
}
