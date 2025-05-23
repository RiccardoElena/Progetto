
package furhatos.app.newskill.model.usecases

import furhatos.app.newskill.model.User
import java.util.UUID

class RetrieveUserUseCase {
    operator fun invoke(userName: String): User? = User(UUID.randomUUID(), userName, "Che squadra tifi?")
}
