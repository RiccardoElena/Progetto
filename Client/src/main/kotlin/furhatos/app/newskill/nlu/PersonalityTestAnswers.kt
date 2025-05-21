package furhatos.app.newskill.nlu

import furhatos.nlu.Intent
import furhatos.util.Language

class PersonalityTestAnswers(
    val answer: PossibleAnswer? = null,
) : Intent() {
    override fun getExamples(lang: Language): List<String> =
        listOf("@answer", "Credo @answer", "Sicuramente @answer", "Probabilmente @answer")
}
