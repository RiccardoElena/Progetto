package furhatos.app.newskill.model.utils.helpers

import furhatos.app.newskill.model.PersonalityDisplacement
import furhatos.app.newskill.model.PersonalityVector
import furhatos.gestures.Gesture
import furhatos.gestures.Gestures

typealias PersonalityDisplacementValues = PersonalityDisplacement.PersonalityDisplacementValues

object GesturesHelper {
    fun gesturesListFromPersonalityVector(personalityVector: PersonalityVector): List<Gesture> =
        gesturesListFromPersonalityVector(
            PersonalityDisplacement(personalityVector),
        )

    fun gesturesListFromPersonalityVector(decodedPersonality: PersonalityDisplacement): List<Gesture> =
        extractExtroversionGestures(decodedPersonality.extroversion) +
            extractAgreeablenessGestures(decodedPersonality.agreeableness) +
            extractConscientiousnessGestures(decodedPersonality.conscientiousness) +
            extractEmotionalStabilityGestures(decodedPersonality.emotionalStability) +
            extractOpennessToExperiencesGestures(decodedPersonality.opennessToExperiences)

    private fun extractExtroversionGestures(extroversionLevel: PersonalityDisplacementValues) =
        when (extroversionLevel) {
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_LOW ->
                listOf(
                    Gestures.GazeAway(strength = 2.0, duration = 2.0),
                    Gestures.ExpressFear(strength = 2.0, duration = 2.0),
                    Gestures.Blink(strength = 2.0, duration = 2.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.LOW ->
                listOf(
                    Gestures.GazeAway,
                    Gestures.ExpressFear,
                    Gestures.Blink,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.MEDIUM -> emptyList()
            PersonalityDisplacement.PersonalityDisplacementValues.HIGH ->
                listOf(
                    Gestures.Smile,
                    Gestures.Nod,
                    Gestures.Surprise,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_HIGH ->
                listOf(
                    Gestures.BigSmile,
                    Gestures.Wink,
                    Gestures.Surprise(strength = 2.0, duration = 2.0),
                )
        }

    private fun extractAgreeablenessGestures(agreeablenessLevel: PersonalityDisplacementValues) =
        when (agreeablenessLevel) {
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_LOW ->
                listOf(
                    Gestures.ExpressAnger(strength = 2.0, duration = 2.0),
                    Gestures.BrowFrown(strength = 2.0, duration = 2.0),
                    Gestures.ExpressDisgust(strength = 2.0, duration = 2.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.LOW ->
                listOf(
                    Gestures.BrowFrown,
                    Gestures.ExpressDisgust,
                    Gestures.Shake,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.MEDIUM ->
                emptyList()
            PersonalityDisplacement.PersonalityDisplacementValues.HIGH ->
                listOf(
                    Gestures.Smile,
                    Gestures.Nod,
                    Gestures.Thoughtful,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_HIGH ->
                listOf(
                    Gestures.BigSmile,
                    Gestures.Surprise(strength = 2.0, duration = 2.0),
                    Gestures.Oh(strength = 2.0, duration = 2.0),
                )
        }

    private fun extractConscientiousnessGestures(conscientiousnessLevel: PersonalityDisplacementValues) =
        when (conscientiousnessLevel) {
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_LOW ->
                listOf(
                    Gestures.GazeAway(strength = 2.0, duration = 2.0),
                    Gestures.Blink(strength = 2.0, duration = 2.0),
                    Gestures.Roll(strength = 2.0, duration = 2.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.LOW ->
                listOf(
                    Gestures.GazeAway,
                    Gestures.Roll,
                    Gestures.Shake,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.MEDIUM ->
                emptyList()
            PersonalityDisplacement.PersonalityDisplacementValues.HIGH ->
                listOf(
                    Gestures.Thoughtful,
                    Gestures.BrowRaise(strength = 1.5, duration = 1.0),
                    Gestures.Nod,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_HIGH ->
                listOf(
                    Gestures.Thoughtful(strength = 2.0, duration = 2.0),
                    Gestures.BrowFrown(strength = 2.0, duration = 1.5),
                    Gestures.OpenEyes(strength = 2.0, duration = 1.0),
                )
        }

    private fun extractEmotionalStabilityGestures(emotionalStabilityLevel: PersonalityDisplacementValues) =
        when (emotionalStabilityLevel) {
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_LOW ->
                listOf(
                    Gestures.ExpressFear(strength = 2.0, duration = 2.0),
                    Gestures.ExpressSad(strength = 2.0, duration = 2.0),
                    Gestures.Roll(strength = 2.0, duration = 2.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.LOW ->
                listOf(
                    Gestures.ExpressSad,
                    Gestures.BrowFrown,
                    Gestures.Shake,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.MEDIUM ->
                emptyList()
            PersonalityDisplacement.PersonalityDisplacementValues.HIGH ->
                listOf(
                    Gestures.Blink,
                    Gestures.Thoughtful,
                    Gestures.OpenEyes(strength = 1.5, duration = 1.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_HIGH ->
                listOf(
                    Gestures.CloseEyes(strength = 2.0, duration = 2.0),
                    Gestures.Smile(strength = 2.0, duration = 1.5),
                    Gestures.Nod(strength = 2.0, duration = 1.0),
                    Gestures.Thoughtful,
                )
        }

    private fun extractOpennessToExperiencesGestures(opennessToExperiencesLevel: PersonalityDisplacementValues) =
        when (opennessToExperiencesLevel) {
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_LOW ->
                listOf(
                    Gestures.GazeAway(strength = 2.0, duration = 2.0),
                    Gestures.ExpressDisgust(strength = 2.0, duration = 2.0),
                    Gestures.BrowRaise(strength = 2.0, duration = 2.0),
                )
            PersonalityDisplacement.PersonalityDisplacementValues.LOW ->
                listOf(
                    Gestures.Roll,
                    Gestures.Blink,
                    Gestures.Shake,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.MEDIUM ->
                emptyList()
            PersonalityDisplacement.PersonalityDisplacementValues.HIGH ->
                listOf(
                    Gestures.OpenEyes(strength = 1.5, duration = 1.0),
                    Gestures.Wink,
                    Gestures.Smile,
                )
            PersonalityDisplacement.PersonalityDisplacementValues.VERY_HIGH ->
                listOf(
                    Gestures.BigSmile(strength = 2.0, duration = 2.0),
                    Gestures.Surprise(strength = 2.0, duration = 1.5),
                    Gestures.Oh(strength = 1.8, duration = 1.0),
                )
        }
}
