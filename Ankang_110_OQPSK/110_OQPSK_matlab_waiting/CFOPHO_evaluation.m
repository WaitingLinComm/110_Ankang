function [] = CFOPHO_evaluation(t, InSig, syncSig, cfo_estimate)

syncSig2 = syncSig .* exp(1i * cfo_estimate * (t : t + length(syncSig)-1));
InSig2 = InSig(t : t+length(syncSig) - 1);

figure; plot(abs(syncSig2)); hold on; plot(abs(InSig2)); legend('syncSig2','InSig2'); title('Ampitude');
figure; plot(abs(syncSig2) ./ (abs(InSig2))); title('Amplitude ratio: abs(syncSig2)./(abs(InSig2))', "Interpreter", "latex");

disp("the cos($\theta$) of InSig and syncSig")
disp(norm(dot(InSig2, syncSig2), 2) / (norm(InSig2, 2) * norm(syncSig2, 2)));

figure; plot(angle(syncSig2)); hold on; plot(angle(InSig2)); legend('syncSig2','InSig2'); title('Angle (rad)');
figure; plot(unwrap(angle(syncSig2))); hold on; plot(unwrap(angle(InSig2))); legend('syncSig2','InSig2'); title('Angle (rad)');
figure; plot(unwrap(angle(syncSig2)) - unwrap(angle(InSig2))); title('phase difference of syncSig2 and InSig2');
end