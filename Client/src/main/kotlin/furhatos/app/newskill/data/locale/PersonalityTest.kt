package furhatos.app.newskill.data.locale

object PersonalityTest {
    private val _questions =
        mapOf(
            1u to "question.1",
            2u to "question.2",
            3u to "question.3",
            4u to "question.4",
            5u to "question.5",
            6u to "question.6",
            7u to "question.7",
            8u to "question.8",
            9u to "question.9",
            10u to "question.10",
        )

    private val _possibleAnswers =
        mapOf(
            1u to "answer.1",
            2u to "answer.2",
            3u to "answer.3",
            4u to "answer.4",
            5u to "answer.5",
            6u to "answer.6",
            7u to "answer.7",
        )

    val possibleAnswers get() = _possibleAnswers
    val questions get() = _questions

    fun textAnswerToNum(answer: String) = _possibleAnswers.entries.firstOrNull { it.value.lowercase() == answer.lowercase() }?.key ?: 0u
}
