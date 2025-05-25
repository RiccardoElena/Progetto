package furhatos.app.newskill.model.utils.helpers

import furhatos.app.newskill.model.PersonalityDisplacement
import furhatos.app.newskill.model.PersonalityVector

object WaitingSentenceHelper {
    private val extrovertExpressions =
        listOf(
            "extrovert.1",
            "extrovert.2",
            "extrovert.3",
            "extrovert.4",
            "extrovert.5",
            "extrovert.6",
        )

    private val agreeableExpressions =
        listOf(
            "agreeable.1",
            "agreeable.2",
            "agreeable.3",
            "agreeable.4",
            "agreeable.5",
            "agreeable.6",
        )

    private val conscientiousExpressions =
        listOf(
            "conscientious.1",
            "conscientious.2",
            "conscientious.3",
            "conscientious.4",
            "conscientious.5",
            "conscientious.6",
        )

    private val stableExpressions =
        listOf(
            "stable.1",
            "stable.2",
            "stable.3",
            "stable.4",
            "stable.5",
            "stable.6",
        )

    private val openExpressions =
        listOf(
            "open.1",
            "open.2",
            "open.3",
            "open.4",
            "open.5",
            "open.6",
        )

    fun waitingSentence(personalityVector: PersonalityVector) = waitingSentence(PersonalityDisplacement(personalityVector))

    fun waitingSentence(personalityDisplacement: PersonalityDisplacement) =
        when (personalityDisplacement.max) {
            0 -> extrovertExpressions
            1 -> agreeableExpressions
            2 -> conscientiousExpressions
            3 -> stableExpressions
            4 -> openExpressions
            else -> emptyList()
        }
}
