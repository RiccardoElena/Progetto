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
        private val _median = PersonalityVector(1.0, 2.0, 3.0, 4.0, 5.0)

        val median get() = _median
    }
}
