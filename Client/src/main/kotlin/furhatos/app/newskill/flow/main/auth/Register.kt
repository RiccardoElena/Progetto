package furhatos.app.newskill.flow.main.auth

import furhatos.app.newskill.model.PersonalityVector
import furhatos.app.newskill.model.User
import furhatos.app.newskill.model.usecases.RetrieveUserUseCase
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state
import java.util.UUID

@Suppress("ktlint:standard:function-naming")
fun Register(personalityVector: PersonalityVector) =
    state {
        onEntry {
            val userName = call(AskUsername) as String
            val (question, answer) = call(AskSecurityQuestion) as Pair<String, String>

            val newUser = User(UUID.randomUUID(), userName, question, personalityVector)

            // regiser(newUser, answer)
            // goto(Conversation(personalityVector))
        }
    }

private val AskUsername =
    state {
        var knownName: String = ""
        onEntry {
            furhat.ask("Qual'è il tuo nome?")
        }

        onReentry {
            furhat.ask("Scusami, conosco già troppe persone col nome $knownName, scegline un altro?")
        }

        onResponse {
            val user = RetrieveUserUseCase()(it.text)

            if (user != null) {
                knownName = it.text
                reentry()
            } else {
                terminate(it.text)
            }
        }
    }

private val AskSecurityQuestion =
    state {
        var question: String = ""
        onEntry {
            furhat.ask(
                "Per poterti riconoscere quando tornerai ho bisogno che tu scelga una domanda di sicurezza," +
                    "quale preferisci tra: " +
                    questions.joinToString(", "),
            )
        }

        onReentry {
            furhat.ask("Bene, e qual'è la risposta?")
        }

        onResponse(cond = { question.isBlank() }) {
            question = it.text
            reentry()
        }

        onResponse {
            terminate(Pair(question, it.text))
        }
    }

private val questions = listOf("Domanda 1", "Domanda 2", "Domanda 3")
