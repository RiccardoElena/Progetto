
package furhatos.app.newskill.model.usecases

import java.util.UUID

class ValidateUserQuestionUseCase {
    operator fun invoke(
        answer: String,
        id: UUID,
    ): Boolean = id != UUID.randomUUID() && answer.lowercase() == "napoli"
}
