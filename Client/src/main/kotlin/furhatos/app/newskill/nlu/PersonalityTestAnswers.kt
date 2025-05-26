package furhatos.app.newskill.nlu

import furhatos.app.newskill.data.locale.Localization
import furhatos.nlu.Intent
import furhatos.util.Language

class PersonalityTestAnswers(
    val answer: PossibleAnswer? = null,
) : Intent() {
    override fun getExamples(lang: Language): List<String> =
        listOf(
            "@answer",
            "${Localization.statelessGetLocalizedString("answer_variant.1", lang)} @answer",
            "${Localization.statelessGetLocalizedString("answer_variant.2", lang)} @answer",
            "${Localization.statelessGetLocalizedString("answer_variant.3", lang)} @answer",
        )
}
