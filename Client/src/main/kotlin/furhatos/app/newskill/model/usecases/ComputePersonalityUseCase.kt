
package furhatos.app.newskill.model.usecases

class ComputePersonalityUseCase {
    operator fun invoke(answers: Map<UInt, UInt> = emptyMap()): List<Double> =
        if (answers.isEmpty()) {
            medianPersonalityVector
        } else {
            computePersonalityVector(answers)
        }

    private fun computePersonalityVector(answers: Map<UInt, UInt>): List<Double> =
        answers
            .mapValues { (k, v) -> if (k % 2u == 0u) 8u - v else v }
            .entries
            .groupBy { it.key % 5u }
            .mapValues { (_, v) -> v.map { it.value.toDouble() }.average() }
            .let {
                (1u until 6u).map { i -> it[i % 5u ] ?: 0.0 }
            }

    private val _medianPersonalityVector = listOf(1.0, 2.0, 3.0, 4.0, 5.0)

    val medianPersonalityVector get() = _medianPersonalityVector
}
