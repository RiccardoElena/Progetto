
package furhatos.app.newskill.model.usecases

import furhatos.app.newskill.model.PersonalityVector

class ComputePersonalityUseCase {
    operator fun invoke(answers: Map<UInt, UInt> = emptyMap()): PersonalityVector =
        if (answers.isEmpty()) {
            PersonalityVector.median
        } else {
            computePersonalityVector(answers)
        }

    private fun computePersonalityVector(answers: Map<UInt, UInt>): PersonalityVector =
        answers
            .mapValues { (k, v) -> if (k % 2u == 0u) 8u - v else v }
            .entries
            .groupBy { it.key % 5u }
            .mapValues { (_, v) -> v.map { it.value.toDouble() }.average() }
            .let {
                PersonalityVector((1u until 6u).map { i -> it[i % 5u] ?: 0.0 })
            }
}
