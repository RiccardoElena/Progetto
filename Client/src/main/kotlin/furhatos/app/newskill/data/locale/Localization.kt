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
        val bundle = ResourceBundle.getBundle("messages", localeMapper(lastLanguage))
        return bundle.getString(key)
    }

    private fun localeMapper(lang: Language): Locale =
        when (lang) {
            Language.FRENCH_CH -> Locale("fr", "FR")
            Language.SPANISH_ES -> Locale("es", "ES")
            Language.ENGLISH_GB -> Locale("en", "EN")
            else -> Locale("default", "Default")
        }

    var lastLanguage: Language = Language.ITALIAN_CH
}
