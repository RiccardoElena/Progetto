package furhatos.app.newskill.nlu

import furhatos.app.newskill.model.PersonalityTest
import furhatos.nlu.EnumEntity
import furhatos.util.Language

class PossibleAnswer : EnumEntity(stemming = true, speechRecPhrases = true) {
    override fun getEnum(lang: Language): List<String> = PersonalityTest.possibleAnswers.values.toList()
}
