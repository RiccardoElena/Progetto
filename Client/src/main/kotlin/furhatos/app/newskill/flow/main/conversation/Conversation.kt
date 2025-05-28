package furhatos.app.newskill.flow.main.conversation

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.data.remote.BackendService
import furhatos.app.newskill.data.remote.Parser
import furhatos.app.newskill.data.remote.dto.ConversationLine
import furhatos.app.newskill.data.remote.dto.MessageHistory
import furhatos.app.newskill.data.remote.dto.Messages
import furhatos.app.newskill.data.remote.dto.Role
import furhatos.app.newskill.flow.partials.BaseWOCatchAll
import furhatos.app.newskill.flow.utils.changeOutputLanguage
import furhatos.app.newskill.model.PersonalityDisplacement
import furhatos.app.newskill.model.PersonalityVector
import furhatos.app.newskill.model.utils.helpers.GesturesHelper
import furhatos.app.newskill.model.utils.helpers.WaitingSentenceHelper
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.flow.kotlin.DialogHistory
import furhatos.flow.kotlin.FlowControlRunner
import furhatos.flow.kotlin.Furhat
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state
import furhatos.gestures.Gesture
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeoutOrNull

@Suppress("ktlint:standard:function-naming")
fun Conversation(furhatPersonality: PersonalityVector) =
    state {
        val gestures = GesturesHelper.gesturesListFromPersonalityVector(furhatPersonality)

        val waitingSentences = WaitingSentenceHelper.waitingSentence(furhatPersonality)
        val scope = CoroutineScope(Dispatchers.IO)
        val channel = Channel<String>(Channel.UNLIMITED)

        onEntry {
            Furhat.dialogHistory.clear()

            BackendService.socket.onMessageReceived = { response ->
                channel.trySend(response)
            }
            furhat.ask(10000, endSil = 2000, maxSpeech = 30000) {
                random {
                    gestures.forEach { +it }
                }
                // placeholder
                +Localization.getLocalizedString("start_prompt")
            }
        }

        include(BaseWOCatchAll)

        onResponse {
            scope.launch {
                if (BackendService.socket.connect()) {
                    BackendService.socket.startListening()

                    if (DEBUG_MODE) {
                        println(
                            Parser.toRequestPayload(
                                pv = PersonalityDisplacement(furhatPersonality),
                                lang = it.language,
                                history = retrieveHistory(),
                            ),
                        )
                    }

                    BackendService.socket.sendMessage(
                        Parser.toRequestPayload(
                            pv = PersonalityDisplacement(furhatPersonality),
                            lang = it.language,
                            history = retrieveHistory(),
                        ),
                    )
                }
            }
            changeOutputLanguage(it.language)
            runBlocking {
                // Invia messaggio

                val r: String? =
                    withTimeoutOrNull(3000) {
                        channel.receive()
                    }

                if (r == null) {
                    furhat.say(async = true) {
                        random {
                            gestures.forEach { gesture -> +gesture }
                        }
                        random {
                            waitingSentences.forEach { sentence -> +Localization.getLocalizedString(sentence) }
                        }
                    }
                }

                // Aspetta la prima risposta con timeout
                val response =
                    r ?: withTimeoutOrNull(2000) {
                        channel.receive()
                    }

                enrichedResponse(response, gestures)
            }
        }
    }

private fun FlowControlRunner.enrichedResponse(
    text: String?,
    gestures: List<Gesture>,
) = text?.let {
    furhat.say(abort = true) {
        text.split(".", "!", "?").forEach { sentence ->
            random {
                gestures.forEach { gesture -> +gesture }
            }
            +sentence
        }
        random {
            gestures.forEach { gesture -> +gesture }
        }
    }
    furhat.listen(10000, endSil = 2000, maxSpeech = 30000)
} ?: run {
    furhat.say(abort = true) {
        random {
            gestures.forEach { gesture -> +gesture }
        }
        +Localization.getLocalizedString("repeat_request")
    }
    furhat.listen(10000, endSil = 2000, maxSpeech = 30000)
}

private fun retrieveHistory(): MessageHistory {
    val contents = mutableListOf<ConversationLine>()
    Furhat.dialogHistory.all.takeLast(5).forEach {
        when (it) {
            is DialogHistory.ResponseItem -> contents.add(toConversationLine(Role.user, it.response.text))
            is DialogHistory.UtteranceItem -> contents.add(toConversationLine(Role.model, it.toText()))
        }
    }
    return MessageHistory(contents)
}

private fun toConversationLine(
    role: Role,
    text: String,
): ConversationLine =
    ConversationLine(
        role,
        listOf(Messages(text)),
    )
