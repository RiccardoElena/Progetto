package furhatos.app.newskill.nlu

import furhatos.nlu.EnumEntity
import furhatos.util.Language

class PossibleLanguages : EnumEntity(stemming = true, speechRecPhrases = true) {
    override fun getEnum(lang: Language): List<String> = listOf("Italiano", "English", "Español", "Français")
}
