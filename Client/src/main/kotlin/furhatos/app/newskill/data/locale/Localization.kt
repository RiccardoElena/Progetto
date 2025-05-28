package furhatos.app.newskill.data.locale

// import furhatos.locale.Locale as FurhatLocale
import furhatos.util.Language
import java.util.Locale
import java.util.ResourceBundle

object Localization {
    fun getLocalizedString(
        key: String,
        lang: Language,
    ): String {
        lastLanguage = lang

        return getLocalizedString(key)
    }

    fun getLocalizedString(key: String): String {
        val bundle = ResourceBundle.getBundle("string", localeMapper(lastLanguage))
        return bundle.getString(key)
    }

    fun statelessGetLocalizedString(
        key: String,
        lang: Language,
    ): String {
        val bundle = ResourceBundle.getBundle("string", localeMapper(lang))
        return bundle.getString(key)
    }

    private fun localeMapper(lang: Language): Locale =
        when (lang.code.lowercase()) {
            "fr-fr" -> Locale.forLanguageTag("fr-FR")
            "es-es" -> Locale.forLanguageTag("es-ES")
            "en-gb" -> Locale.forLanguageTag("en-EN")
            else -> Locale.forLanguageTag("default-Default")
        }

    var lastLanguage: Language = Language.ITALIAN_CH
}
