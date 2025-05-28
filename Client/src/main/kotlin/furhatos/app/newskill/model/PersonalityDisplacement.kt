package furhatos.app.newskill.model

import kotlin.math.abs

class PersonalityDisplacement(
    personalityVector: PersonalityVector,
) {
    private val personalityArray =
        arrayOf(
            personalityVector.extroversion,
            personalityVector.agreeableness,
            personalityVector.conscientiousness,
            personalityVector.emotionalStability,
            personalityVector.opennessToExperiences,
        )

    enum class PersonalityDisplacementValues(
        val code: Int,
    ) {
        VERY_LOW(-2),
        LOW(-1),
        MEDIUM(0),
        HIGH(1),
        VERY_HIGH(2),
        ;

        companion object {
            val map = values().associateBy(PersonalityDisplacementValues::code)

            fun displacementFromCode(code: Int): PersonalityDisplacementValues = map[code] ?: MEDIUM
        }
    }

    val extroversion: PersonalityDisplacementValues =
        computeDisplacement(
            personalityVector.extroversion,
            PersonalityVector.stdDev.extroversion,
            PersonalityVector.median.extroversion,
        )

    val agreeableness =
        computeDisplacement(
            personalityVector.agreeableness,
            PersonalityVector.stdDev.agreeableness,
            PersonalityVector.median.agreeableness,
        )
    val conscientiousness =
        computeDisplacement(
            personalityVector.conscientiousness,
            PersonalityVector.stdDev.conscientiousness,
            PersonalityVector.median.conscientiousness,
        )
    val emotionalStability =
        computeDisplacement(
            personalityVector.emotionalStability,
            PersonalityVector.stdDev.emotionalStability,
            PersonalityVector.median.emotionalStability,
        )
    val opennessToExperiences =
        computeDisplacement(
            personalityVector.opennessToExperiences,
            PersonalityVector.stdDev.opennessToExperiences,
            PersonalityVector.median.opennessToExperiences,
        )

    companion object {
        private fun computeDisplacement(
            value: Double,
            stdDev: Double,
            median: Double,
        ): PersonalityDisplacementValues {
            val sign = if (value > median) 1 else -1

            val delta = abs(value - median)

            if (delta < stdDev) return PersonalityDisplacementValues.displacementFromCode(0)
            if (delta < 2 * stdDev) return PersonalityDisplacementValues.displacementFromCode(sign * 1)
            return PersonalityDisplacementValues.displacementFromCode(sign * 2)
        }
    }

    val max = personalityArray.indexOf(personalityArray.max())
}
