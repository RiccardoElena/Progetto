package furhatos.app.newskill.nlu

import furhatos.app.newskill.data.locale.Localization
import furhatos.nlu.Intent
import furhatos.util.Language

class Language(
    val language: PossibleLanguages? = null,
) : Intent() {
    override fun getExamples(lang: Language): List<String> =
        listOf(
            "@language",
            "${Localization.getLocalizedString("answer_variant.1", lang)} @language",
            "${Localization.getLocalizedString("answer_variant.2", lang)} @language",
            "${Localization.getLocalizedString("answer_variant.3", lang)} @language",
        )
}
