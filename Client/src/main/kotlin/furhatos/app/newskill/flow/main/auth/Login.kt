package furhatos.app.newskill.flow.main.auth

import furhatos.app.newskill.flow.betterYN
import furhatos.app.newskill.flow.main.Idle
import furhatos.app.newskill.flow.main.personalityTest.TestStart
import furhatos.app.newskill.model.User
import furhatos.app.newskill.model.usecases.RetrieveUserUseCase
import furhatos.app.newskill.model.usecases.ValidateUserQuestionUseCase
import furhatos.flow.kotlin.FlowControlRunner
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state

val Login =
    state {
        onEntry {
            val user = call(AskUsername) as User
            call(AskSecurityQuestion(user))
            // goto(Conversation(user.personalityVector))
        }
    }

@Suppress("ktlint:standard:function-naming")
private val AskUsername =
    state {
        var attempts = 1
        onEntry {
            furhat.ask("Qual'è il tuo nome?")
        }

        onReentry {
            if (attempts < 4) {
                furhat.ask("Non conosco nessuno che si chiama così, puoi provare a ripetere il tuo nome?")
            } else {
                tooManyAttempts("Scusa, ma temo di non conoscerti o di non ricordarmi di te")
            }
        }

        onResponse {
            val user = RetrieveUserUseCase()(it.text)

            if (user != null) {
                terminate(user)
            } else {
                attempts++
                reentry()
            }
        }
    }

@Suppress("ktlint:standard:function-naming")
private fun AskSecurityQuestion(user: User) =
    state {
        var attempts = 1
        onEntry {
            furhat.ask(user.securityQuestion)
        }

        onReentry {
            if (attempts < 3) {
                furhat.ask("Non mi hai dato questa risposta l'ultima volta, riprova!")
            } else if (attempts == 4) {
                furhat.ask("Proprio sicuro di essere tu? Prova un ultima volta")
            } else {
                tooManyAttempts("Mi sa che non sei ${user.userName} che conosco")
            }
        }

        onResponse(cond = { ValidateUserQuestionUseCase()(it.text, user.id) }) {
            terminate()
        }

        onResponse {
            attempts++
            reentry()
        }
    }

private fun FlowControlRunner.tooManyAttempts(message: String): Nothing {
    furhat.say("$message, ma sono felice di fare nuove conoscenze\"")
    if (betterYN("Vuoi rispondere a qualche domanda su di te?")) {
        goto(TestStart)
    } else {
        furhat.say("Capisco, non fa niente. Quando vuoi sono qui")
        goto(Idle)
    }
}
