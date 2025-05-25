package furhatos.app.newskill.nlu

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.data.locale.PersonalityTest
import furhatos.nlu.EnumEntity
import furhatos.util.Language

class PossibleAnswer : EnumEntity(stemming = true, speechRecPhrases = true) {
    override fun getEnum(lang: Language): List<String> =
        PersonalityTest.possibleAnswers
            .values
            .toList()
            .map { Localization.getLocalizedString(it, lang) }
}
