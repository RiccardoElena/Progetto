package furhatos.app.newskill.data.locale

object PersonalityTest {
    private val _questions =
        mapOf(
            1u to "Estroversa, esuberante",
            2u to "Polemica, litigiosa",
            3u to "Affidabile, auto-disciplinata",
            4u to "Ansiosa, che si agita facilmente",
            5u to "Aperta alle nuove esperienze, con molti interessi",
            6u to "Riservata, silenziosa",
            7u to "Comprensiva, affettuosa",
            8u to "Disorganizzata, distratta",
            9u to "Tranquilla, emotivamente stabile",
            10u to "Tradizionalista, abitudinaria",
        )

    private val _possibleAnswers =
        mapOf(
            1u to "Completamente in disaccordo",
            2u to "Molto in disaccordo",
            3u to "Un po' in disaccordo",
            4u to "Né d'accordo né in disaccordo",
            5u to "Un po' d'accordo",
            6u to "Molto d'accordo",
            7u to "Completamente d'accordo",
        )

    val possibleAnswers get() = _possibleAnswers
    val questions get() = _questions

    fun textAnswerToNum(answer: String) = _possibleAnswers.entries.firstOrNull { it.value.lowercase() == answer.lowercase() }?.key ?: 0u
}
