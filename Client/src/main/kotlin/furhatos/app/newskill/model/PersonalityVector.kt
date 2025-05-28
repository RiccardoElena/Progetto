package furhatos.app.newskill.model

data class PersonalityVector(
    val extroversion: Double,
    val agreeableness: Double,
    val conscientiousness: Double,
    val emotionalStability: Double,
    val opennessToExperiences: Double,
) {
    constructor(l: List<Double>) : this(
        extroversion = l[0],
        agreeableness = l[1],
        conscientiousness = l[2],
        emotionalStability = l[3],
        opennessToExperiences = l[4],
    )

    constructor(vararg l: Double) : this(
        l.toList(),
    )

    companion object {
        private val _median =
            PersonalityVector(
                4.44,
                5.23,
                5.4,
                4.83,
                5.38,
            )

        private val _stdDev =
            PersonalityVector(
                1.45,
                1.11,
                1.32,
                1.42,
                1.07,
            )

        /**
         * Median values taken from [here](https://gosling.psy.utexas.edu/scales-weve-developed/ten-item-personality-measure-tipi/)
         *
         * The median used is the general one reported in the Excel spreadsheet.
         * More demographically refined medians exists for the same study, but are not implemented yet.
         * */
        val median get() = _median

        /**
         * Standard deviation values taken from [here](https://gosling.psy.utexas.edu/scales-weve-developed/ten-item-personality-measure-tipi/)
         *
         * The standard deviation used is the general one reported in the Excel spreadsheet.
         * More demographically refined standard deviations exists for the same study, but are not implemented yet.
         * Generally speaking the standard deviation used here differs from the more specific ones by an error of ±0.1
         * */
        val stdDev get() = _stdDev

        fun symmetricVector(vector: PersonalityVector): PersonalityVector =
            PersonalityVector(
                extroversion = 2 * _median.extroversion - vector.extroversion,
                agreeableness = 2 * _median.agreeableness - vector.agreeableness,
                conscientiousness = 2 * _median.conscientiousness,
                emotionalStability = 2 * _median.emotionalStability - vector.emotionalStability,
                opennessToExperiences = 2 * _median.opennessToExperiences - vector.opennessToExperiences,
            )
    }

    fun symmetricVector(): PersonalityVector = symmetricVector(this)
}
