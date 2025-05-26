package furhatos.app.newskill.nlu

import furhatos.app.newskill.data.locale.Localization
import furhatos.nlu.Intent
import furhatos.util.Language

class ChangeLanguage : Intent() {
    override fun getExamples(lang: Language): List<String> =
        listOf(
            Localization.statelessGetLocalizedString("language_option.1", lang),
            Localization.statelessGetLocalizedString("language_option.2", lang),
            Localization.statelessGetLocalizedString("language_option.3", lang),
            Localization.statelessGetLocalizedString("language_option.4", lang),
        )
}
