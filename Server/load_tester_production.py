#!/usr/bin/env python3
"""
Sustained Load Tester per Robot Dialog Server
Simula carichi sostenuti realistici per testare l'auto-scaling del thread pool
"""

import socket
import threading
import time
import random
import json
import argparse
from concurrent.futures import ThreadPoolExecutor
import statistics
import math

class SustainedLoadTester:
    def __init__(self, host='localhost', port=8081):
        self.host = host
        self.port = port
        self.results = []
        self.active_clients = 0
        self.lock = threading.Lock()
        self.running = True
        
    def generate_realistic_personality(self):
        """Genera personalità più realistiche con distribuzione normale"""
        # Distribuzione più realistica intorno a valori medi
        traits = {
            'extraversion': max(1.0, min(7.0, random.normalvariate(4.0, 1.2))),
            'agreeableness': max(1.0, min(7.0, random.normalvariate(5.0, 1.0))),
            'conscientiousness': max(1.0, min(7.0, random.normalvariate(4.5, 1.1))),
            'emotional_stability': max(1.0, min(7.0, random.normalvariate(4.2, 1.3))),
            'openness': max(1.0, min(7.0, random.normalvariate(4.8, 1.2)))
        }
        return f"{traits['extraversion']:.1f},{traits['agreeableness']:.1f},{traits['conscientiousness']:.1f},{traits['emotional_stability']:.1f},{traits['openness']:.1f}"
    
    def generate_realistic_conversation(self):
        """Genera conversazioni più complesse per creare carico sostenuto"""
        # Mix di richieste semplici e complesse per simulare utilizzo reale
        simple_messages = [
            "Hello, how are you today?",
            "What's your name?",
            "How can you help me?",
            "Tell me a joke",
            "What's the weather like?"
        ]
        
        complex_messages = [
            "I'm feeling anxious about my upcoming job interview. Can you help me prepare and give me some confidence-building advice?",
            "Explain the concept of machine learning in a way that a 12-year-old would understand, using fun analogies.",
            "I'm having trouble sleeping lately due to stress. What are some natural relaxation techniques you'd recommend?",
            "Tell me a creative story about a robot who learns to paint, and make it emotionally engaging.",
            "Help me understand the philosophical implications of artificial intelligence consciousness and free will.",
            "I'm learning Italian and struggling with verb conjugations. Can you explain the pattern for regular -are verbs?",
            "My elderly grandmother is lonely and I want to cheer her up. Suggest some heartwarming conversation topics.",
            "Analyze the pros and cons of remote work versus office work in the post-pandemic era."
        ]
        
        # 70% richieste semplici, 30% complesse (simula utilizzo reale)
        if random.random() < 0.7:
            message = random.choice(simple_messages)
        else:
            message = random.choice(complex_messages)
        
        conversation = [{
            "role": "user",
            "parts": [{"text": message}]
        }]
        return json.dumps(conversation)
    
    def simulate_user_session(self, client_id, session_duration, requests_per_minute):
        """Simula una sessione utente realistica con multiple richieste"""
        session_start = time.time()
        request_count = 0
        
        print(f"[Client {client_id}] Starting session: {session_duration}s, {requests_per_minute} req/min")
        
        with self.lock:
            self.active_clients += 1
        
        try:
            while time.time() - session_start < session_duration and self.running:
                # Calcola intervallo tra richieste
                interval = 60.0 / requests_per_minute
                
                # Aggiungi variazione realistica (±30%)
                actual_interval = interval * random.uniform(0.7, 1.3)
                
                # Fai la richiesta
                self.single_ai_request(f"{client_id}-{request_count}")
                request_count += 1
                
                # Pausa prima della prossima richiesta
                if time.time() - session_start < session_duration:
                    time.sleep(actual_interval)
                    
        finally:
            with self.lock:
                self.active_clients -= 1
            
        print(f"[Client {client_id}] Session ended: {request_count} requests in {time.time() - session_start:.1f}s")
    
    def single_ai_request(self, client_id):
        """Esegue una singola richiesta AI realistica"""
        start_time = time.time()
        result = {
            'client_id': client_id,
            'success': False,
            'response_time': 0,
            'error': None,
            'response_length': 0
        }
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(45)  # Timeout più lungo per richieste AI
            sock.connect((self.host, self.port))
            
            # Genera richiesta AI realistica
            personality = self.generate_realistic_personality()
            language = random.choice(['en', 'it', 'es', 'fr'])  # Mix linguaggi
            conversation = self.generate_realistic_conversation()
            
            # USA SEMPRE MSG_AI_DIALOG_REQUEST (tipo 1) per carico sostenuto
            request = f"1|{personality}|{language}|{conversation}\n"
            
            sock.send(request.encode())
            response = sock.recv(4096).decode().strip()
            
            end_time = time.time()
            result['success'] = True
            result['response_time'] = (end_time - start_time) * 1000
            result['response_length'] = len(response)
            
            if len(client_id) < 20:  # Evita log spam
                print(f"[{client_id}] Response ({result['response_time']:.0f}ms): {response[:60]}...")
            
            sock.close()
            
        except Exception as e:
            result['error'] = str(e)
            if "timed out" not in str(e):  # Log solo errori non-timeout
                print(f"[{client_id}] ERROR: {e}")
                
        with self.lock:
            self.results.append(result)
            
        return result
    
    def run_gradual_ramp_test(self, duration_minutes=10):
        """Test con crescita graduale del carico (simula crescita utenti)"""
        print(f"=== GRADUAL RAMP TEST ({duration_minutes} minutes) ===")
        print("Simulates realistic user growth pattern")
        print("-" * 60)
        
        start_time = time.time()
        duration_seconds = duration_minutes * 60
        
        def user_spawner():
            """Spawna nuovi utenti gradualmente"""
            spawned_users = 0
            
            while time.time() - start_time < duration_seconds and self.running:
                # Crescita graduale: più utenti col tempo
                elapsed_ratio = (time.time() - start_time) / duration_seconds
                target_concurrent = int(2 + elapsed_ratio * 15)  # Da 2 a 17 utenti
                
                current_active = self.active_clients
                
                if current_active < target_concurrent:
                    # Spawna nuovo utente
                    spawned_users += 1
                    session_duration = random.uniform(120, 300)  # 2-5 minuti per sessione
                    requests_per_minute = random.uniform(3, 8)   # 3-8 richieste/minuto
                    
                    thread = threading.Thread(
                        target=self.simulate_user_session,
                        args=(f"ramp-{spawned_users}", session_duration, requests_per_minute)
                    )
                    thread.daemon = True
                    thread.start()
                
                print(f"[RAMP] Active users: {self.active_clients}, Target: {target_concurrent}")
                time.sleep(10)  # Check ogni 10 secondi
        
        # Avvia lo spawner
        spawner_thread = threading.Thread(target=user_spawner)
        spawner_thread.daemon = True
        spawner_thread.start()
        
        # Attendi fine test
        time.sleep(duration_seconds)
        self.running = False
        
        # Aspetta che tutti i client finiscano
        print("[RAMP] Waiting for active sessions to complete...")
        while self.active_clients > 0:
            time.sleep(2)
            print(f"[RAMP] Still active: {self.active_clients} clients")
        
        total_time = time.time() - start_time
        self.print_statistics(total_time, "GRADUAL RAMP")
    
    def run_steady_state_test(self, concurrent_users=12, duration_minutes=8):
        """Test con carico sostenuto costante"""
        print(f"=== STEADY STATE TEST ({concurrent_users} users, {duration_minutes} minutes) ===")
        print("Simulates sustained production load")
        print("-" * 60)
        
        start_time = time.time()
        duration_seconds = duration_minutes * 60
        
        # Spawna utenti con sessioni sovrapposte
        threads = []
        for i in range(concurrent_users):
            # Sessioni lunghe che si sovrappongono
            session_duration = duration_seconds + random.uniform(0, 60)
            requests_per_minute = random.uniform(4, 10)  # Varia il carico per utente
            
            thread = threading.Thread(
                target=self.simulate_user_session,
                args=(f"steady-{i}", session_duration, requests_per_minute)
            )
            thread.daemon = True
            threads.append(thread)
        
        # Avvia tutti i thread con piccoli ritardi
        for i, thread in enumerate(threads):
            thread.start()
            time.sleep(random.uniform(2, 8))  # Stagger startup
        
        # Monitor progress
        while time.time() - start_time < duration_seconds:
            print(f"[STEADY] Active users: {self.active_clients}, Elapsed: {(time.time() - start_time)/60:.1f}m")
            time.sleep(15)
        
        self.running = False
        
        # Aspetta fine sessioni
        print("[STEADY] Waiting for sessions to complete...")
        while self.active_clients > 0:
            time.sleep(2)
            print(f"[STEADY] Still active: {self.active_clients} clients")
        
        total_time = time.time() - start_time
        self.print_statistics(total_time, "STEADY STATE")
    
    def run_peak_hours_test(self, duration_minutes=12):
        """Test che simula ore di punta con variazioni"""
        print(f"=== PEAK HOURS TEST ({duration_minutes} minutes) ===")
        print("Simulates realistic daily usage patterns with peaks and valleys")
        print("-" * 60)
        
        start_time = time.time()
        duration_seconds = duration_minutes * 60
        
        def dynamic_load_manager():
            """Gestisce carico dinamico con pattern realistici"""
            spawned_users = 0
            
            while time.time() - start_time < duration_seconds and self.running:
                elapsed = time.time() - start_time
                progress = elapsed / duration_seconds
                
                # Pattern sinusoidale con picchi casuali
                base_load = 8 + 6 * math.sin(progress * 4 * math.pi)  # 2-14 utenti base
                spike_factor = 1.0
                
                # Picchi casuali (20% probabilità ogni 30 secondi)
                if random.random() < 0.05:  # 5% ogni check
                    spike_factor = random.uniform(1.5, 2.2)
                    print(f"[PEAK] Load spike! Factor: {spike_factor:.1f}")
                
                target_users = int(base_load * spike_factor)
                current_active = self.active_clients
                
                if current_active < target_users:
                    # Aggiungi utenti
                    for _ in range(min(3, target_users - current_active)):
                        spawned_users += 1
                        session_duration = random.uniform(90, 240)  # 1.5-4 min
                        requests_per_minute = random.uniform(2, 12)  # Varia molto
                        
                        thread = threading.Thread(
                            target=self.simulate_user_session,
                            args=(f"peak-{spawned_users}", session_duration, requests_per_minute)
                        )
                        thread.daemon = True
                        thread.start()
                
                print(f"[PEAK] Active: {self.active_clients}, Target: {target_users}, Progress: {progress*100:.0f}%")
                time.sleep(8)  # Check più frequenti
        
        # Avvia load manager
        manager_thread = threading.Thread(target=dynamic_load_manager)
        manager_thread.daemon = True
        manager_thread.start()
        
        time.sleep(duration_seconds)
        self.running = False
        
        # Cleanup
        print("[PEAK] Waiting for sessions to complete...")
        while self.active_clients > 0:
            time.sleep(2)
            print(f"[PEAK] Still active: {self.active_clients} clients")
        
        total_time = time.time() - start_time
        self.print_statistics(total_time, "PEAK HOURS")
    
    def print_statistics(self, total_time, test_type):
        """Stampa statistiche dettagliate del test"""
        if not self.results:
            print("No results to analyze")
            return
        
        successful = [r for r in self.results if r['success']]
        failed = [r for r in self.results if not r['success']]
        
        response_times = [r['response_time'] for r in successful]
        
        print(f"\n{'=' * 60}")
        print(f"{test_type} TEST RESULTS")
        print(f"{'=' * 60}")
        print(f"Test duration: {total_time:.1f}s")
        print(f"Total requests: {len(self.results)}")
        print(f"Successful: {len(successful)} ({len(successful)/len(self.results)*100:.1f}%)")
        print(f"Failed: {len(failed)} ({len(failed)/len(self.results)*100:.1f}%)")
        print(f"Average RPS: {len(self.results)/total_time:.1f}")
        print(f"Sustained load achieved: {len(successful)/total_time:.1f} successful RPS")
        
        if response_times:
            print(f"\nResponse Time Statistics:")
            print(f"  Min: {min(response_times):.0f}ms")
            print(f"  Max: {max(response_times):.0f}ms")  
            print(f"  Avg: {statistics.mean(response_times):.0f}ms")
            print(f"  Median: {statistics.median(response_times):.0f}ms")
            print(f"  95th percentile: {sorted(response_times)[int(len(response_times) * 0.95)]:.0f}ms")
        
        if failed:
            print(f"\nError Summary:")
            error_counts = {}
            for r in failed:
                error = r['error'][:50] if r['error'] else 'Unknown'
                error_counts[error] = error_counts.get(error, 0) + 1
            
            for error, count in error_counts.items():
                print(f"  {error}: {count} times")
        
        print(f"\nTHREAD POOL SCALING OBSERVATIONS:")
        print(f"  - Monitor server logs for auto-scaling events")
        print(f"  - Look for 'SCALING UP' and 'SCALING DOWN' messages")
        print(f"  - Check thread pool statistics printed every 30s")
        print(f"{'=' * 60}\n")

def main():
    parser = argparse.ArgumentParser(description='Sustained Load Tester for Robot Dialog Server')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=8081, help='Server port')
    parser.add_argument('--test', choices=['ramp', 'steady', 'peak'], default='steady',
                        help='Test type: ramp (gradual growth), steady (constant load), peak (variable load)')
    parser.add_argument('--duration', type=int, default=10, help='Test duration in minutes')
    parser.add_argument('--users', type=int, default=12, help='Concurrent users for steady test')
    
    args = parser.parse_args()
    
    tester = SustainedLoadTester(args.host, args.port)
    
    print(f"🤖 SUSTAINED LOAD TESTER for Robot Dialog Server")
    print(f"Target: {args.host}:{args.port}")
    print(f"This tester uses REAL AI requests to create sustained load")
    print(f"Perfect for testing thread pool auto-scaling behavior")
    print("=" * 60)
    
    try:
        if args.test == 'ramp':
            tester.run_gradual_ramp_test(args.duration)
        elif args.test == 'steady':
            tester.run_steady_state_test(args.users, args.duration)
        elif args.test == 'peak':
            tester.run_peak_hours_test(args.duration)
    except KeyboardInterrupt:
        print("\n⚠️  Test interrupted by user")
        tester.running = False
        time.sleep(2)

if __name__ == '__main__':
    main()
