package furhatos.app.newskill.model.usecases

import furhatos.app.newskill.model.PersonalityVector
import kotlin.random.Random

class RetrievePersonalityVectorUseCase {
    operator fun invoke(
        personalityVector: PersonalityVector,
        equal: Boolean,
    ) = if (Random.nextBoolean()) {
        personalityVector
    } else {
        personalityVector.symmetricVector()
    }
}
